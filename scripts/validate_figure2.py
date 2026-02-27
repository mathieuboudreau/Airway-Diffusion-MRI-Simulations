#!/usr/bin/env python3
"""
validate_figure2.py
===================
Python translation of the original MATLAB analysis scripts used to generate
Figure 2 of Boudreau et al., Magn Reson Med (2012).

Workflow
--------
1.  Compute the 8 gradient amplitudes (T/m) from target b-values (s/cm²)
    using the trapezoidal PGSE b-value formula (Eq. 2 in the paper).
2.  Load the C-simulation output files — one file per gradient amplitude,
    each containing data for all 30 gradient orientations.
3.  Compute the orientation-weighted apparent diffusion coefficient (ADC)
    for each b-value.
4.  Fit DL and DT to the reconstructed normalised signal using Eq. 3.

Output-format notes
-------------------
Old C code  (3 columns):  angle_idx, angle_rad, abs_signal
    Rows are t-major: for every timestep, all 30 angles appear in order.
    Equivalent to MATLAB: textread then reshape(c, length(c)/30, 30),
    so C(1,:) = t=1 signal, C(end,:) = t=Nt signal.

New C code  (5 columns):  angle_idx, angle_rad, timestep, real_signal, abs_signal
    Rows are angle-major: all timesteps for angle 1 first, then angle 2, …
    Both formats are auto-detected and handled transparently.

Usage
-----
    # Dry run — print gradient amplitudes only:
    python validate_figure2.py

    # Healthy geometry (8 output files, one per b-value):
    python validate_figure2.py b1.txt b2.txt b3.txt b4.txt b5.txt b6.txt b7.txt b8.txt

    # Compare healthy vs diseased and check paper headline numbers:
    python validate_figure2.py \\
        --healthy  h_b1.txt h_b2.txt h_b3.txt h_b4.txt h_b5.txt h_b6.txt h_b7.txt h_b8.txt \\
        --diseased d_b1.txt d_b2.txt d_b3.txt d_b4.txt d_b5.txt d_b6.txt d_b7.txt d_b8.txt \\
        --check

Reference
---------
Boudreau M, Xu X, Santyr GE.
"Measurement of 129Xe Gas Apparent Diffusion Coefficient Anisotropy in an
Elastase-Instilled Rat Model of Emphysema."
Magn Reson Med, 2012.  DOI 10.1002/mrm.24224
"""

import argparse
import sys
from collections import defaultdict

import numpy as np
from scipy.optimize import curve_fit
from scipy.special import erf


# ---------------------------------------------------------------------------
# Physical constants
# ---------------------------------------------------------------------------

# Xe-129 gyromagnetic ratio (rad/s/T).
# The original MATLAB used 3.14159 (not the full np.pi) — reproduced here
# for fidelity.
GAMMA = -11_860_390.0 * 2.0 * 3.14159


# ---------------------------------------------------------------------------
# Pulse parameters  (from the original grad_calc.m)
# ---------------------------------------------------------------------------

TAU      = 1.00e-3   # gradient ramp up/down time  τ  (s)
PLATFORM = 8.00e-3   # flat-top duration                (s)
RESTTIME = 0.0       # time between the two lobes       (s)

SMALL_DELTA = 2.0 * TAU + PLATFORM      # total pulse duration  δ  (s)
BIG_DELTA   = SMALL_DELTA + RESTTIME    # diffusion time        Δ  (s)


# ---------------------------------------------------------------------------
# b-value formula  (Eq. 2 in the paper, trapezoidal PGSE)
#
#   b = (γ G_m)² [ δ²(Δ − δ/3)  +  τ(δ² − 2Δδ + Δτ − 7/6·δτ + 8/15·τ²) ]
#
# This matches the MATLAB expression in grad_calc.m exactly.
# ---------------------------------------------------------------------------

def _bvalue_denom(tau, small_delta, big_delta):
    """Inner bracketed term of Eq. 2 (units: s³)."""
    return (
        small_delta**2 * (big_delta - small_delta / 3.0)
        + tau * (
            small_delta**2
            - 2.0 * big_delta * small_delta
            + big_delta * tau
            - 7.0 * small_delta * tau / 6.0
            + 8.0 * tau**2 / 15.0
        )
    )


_DENOM = _bvalue_denom(TAU, SMALL_DELTA, BIG_DELTA)


def gradients_from_b(b_scm2, gamma=GAMMA, denom=_DENOM):
    """
    Return gradient amplitudes (T/m) for target b-values (s/cm²).

    Mirrors grad_calc.m:
        grad = sqrt(b * 100^2 / (gamma^2 * denom))
    where 100^2 converts b from s/cm² to s/m².
    """
    b_si = np.asarray(b_scm2, dtype=float) * 1e4   # s/cm² → s/m²
    return np.sqrt(b_si / (gamma**2 * denom))


def b_from_gradients(gamp, gamma=GAMMA, denom=_DENOM):
    """
    Return b-values (s/cm²) for given gradient amplitudes (T/m).

    Mirrors the ADC-script b-value line:
        b = 1e-4 * gamma^2 * maxgrad^2 * denom
    """
    return gamma**2 * np.asarray(gamp, dtype=float)**2 * denom * 1e-4


# ---------------------------------------------------------------------------
# Simulation output parsers
# ---------------------------------------------------------------------------

def _detect_ncols(filename):
    """Return the number of comma-separated columns in the first data line."""
    with open(filename) as fh:
        for line in fh:
            line = line.strip()
            if line:
                return len(line.split(","))
    return 0


def _load_new_format(filename, n_angles):
    """
    Parse the new 5-column format produced by run_bloch_torrey():
        angle_idx, angle_rad, timestep, real_signal, abs_signal

    Rows are angle-major.  Returns sig_ini and sig_fin, each shape (n_angles,),
    holding |S| at t=1 and t=Nt respectively.
    """
    by_angle = defaultdict(dict)   # {angle_idx: {timestep: abs_signal}}

    with open(filename) as fh:
        for line in fh:
            parts = line.strip().split(",")
            if len(parts) != 5:
                continue
            ai = int(parts[0])
            t  = int(parts[2])
            ab = float(parts[4])
            by_angle[ai][t] = ab

    sig_ini = np.zeros(n_angles)
    sig_fin = np.zeros(n_angles)

    for ai in sorted(by_angle.keys()):
        t_vals = sorted(by_angle[ai].keys())
        sig_ini[ai - 1] = by_angle[ai][t_vals[0]]    # t = 1
        sig_fin[ai - 1] = by_angle[ai][t_vals[-1]]   # t = Nt

    return sig_ini, sig_fin


def _load_old_format(filename, n_angles):
    """
    Parse the old 3-column format:
        angle_idx, angle_rad, abs_signal

    Rows are t-major (all n_angles angles for t=1, then t=2, …), matching the
    original MATLAB reshape(c, length(c)/30, 30).  Returns sig_ini and sig_fin.
    """
    abs_signals = []

    with open(filename) as fh:
        for line in fh:
            parts = line.strip().split(",")
            if len(parts) != 3:
                continue
            abs_signals.append(float(parts[2]))

    n_total = len(abs_signals)
    if n_total % n_angles != 0:
        raise ValueError(
            f"{filename}: {n_total} rows not divisible by n_angles={n_angles}"
        )

    # Reshape to (Nt × n_angles) — identical to MATLAB reshape(c, length(c)/30, 30)
    matrix = np.array(abs_signals).reshape(-1, n_angles)

    sig_ini = matrix[0, :]    # first row  = t=1 signal for all angles
    sig_fin = matrix[-1, :]   # last  row  = t=Nt signal for all angles

    return sig_ini, sig_fin


def load_signals(filename, n_angles=30):
    """Auto-detect the output format and return (sig_ini, sig_fin)."""
    ncols = _detect_ncols(filename)
    if ncols == 5:
        return _load_new_format(filename, n_angles)
    elif ncols == 3:
        return _load_old_format(filename, n_angles)
    else:
        raise ValueError(f"{filename}: unexpected column count ({ncols})")


# ---------------------------------------------------------------------------
# DL / DT fitting model  (Eq. 3 in the paper)
# ---------------------------------------------------------------------------

def eq3(b, DL, DT):
    """
    Anisotropic diffusion signal model (Eq. 3, Boudreau et al. 2012):

        S/S0 = exp(-b·DT) · √(π / (4b(DL−DT))) · erf(√(b(DL−DT)))

    The original MATLAB myfunDLDTFit writes the same expression as a product
    of two exp() factors; they simplify to exp(-b·DT), so the forms are
    algebraically identical.
    """
    x = np.sqrt(np.maximum(b * (DL - DT), 1e-30))
    return np.exp(-b * DT) * (np.sqrt(np.pi) / (2.0 * x)) * erf(x)


# ---------------------------------------------------------------------------
# Main analysis  (mirrors the MATLAB ADC-analysis script)
# ---------------------------------------------------------------------------

def analyze(filenames, b_values_scm2, n_angles=30):
    """
    Compute DL and DT from simulation output files.

    Parameters
    ----------
    filenames      : sequence of 8 paths, one per b-value
    b_values_scm2  : sequence of 8 b-values in s/cm², matching filenames
    n_angles       : number of gradient orientations (default 30)

    Returns
    -------
    DL, DT   : longitudinal / transverse diffusion coefficients (cm²/s)
    signorm  : (8,) reconstructed angle-averaged normalised signal used for fit
    """
    # sin-weighting for random cylinder orientation in 3D — mirrors MATLAB:
    #   ALPHA = [1:30]*pi/30;   alpha = sin(ALPHA);
    ALPHA = np.arange(1, n_angles + 1) * np.pi / n_angles
    alpha = np.sin(ALPHA)

    b   = np.asarray(b_values_scm2, dtype=float)
    ADC = np.zeros(len(b))

    for i, (fname, bi) in enumerate(zip(filenames, b)):
        sig_ini, sig_fin = load_signals(fname, n_angles)
        ratio  = sig_fin / sig_ini
        # Angle-weighted ADC — direct translation of the MATLAB lines:
        #   ADC(i) = (1/b(i)) * (log(sum(alpha)) - log(sum(alpha.*(sigfin./sigini))))
        ADC[i] = (np.log(np.sum(alpha)) - np.log(np.sum(alpha * ratio))) / bi

    signorm = np.exp(-ADC * b)   # reconstructed normalised signal vs b

    # Fit DL and DT — same initial guess and bounds as MATLAB lsqcurvefit call:
    #   x0 = [0.06, 0.01];   bounds = ([0.00001, 0.00001], [0.061, 0.061])
    x0     = [0.06, 0.01]
    bounds = ([1e-5, 1e-5], [0.061, 0.061])
    popt, _ = curve_fit(eq3, b, signorm, p0=x0, bounds=bounds)
    DL, DT  = popt

    return DL, DT, signorm


# ---------------------------------------------------------------------------
# Validation against paper headline numbers  (Figure 2)
# ---------------------------------------------------------------------------

def check_paper_results(DL_h, DT_h, DL_d, DT_d, tol_pct=15.0):
    """
    Verify the two headline percentage differences from Figure 2:
      - DT increase (diseased vs healthy) ≈ 159 %  at Δ = 5 ms
      - DL increase (diseased vs healthy) ≈  53 %  at Δ = 50 ms

    tol_pct : absolute tolerance in percentage points (default ±15 pp).

    Returns True if both checks pass.
    """
    ok = True
    checks = [
        ("DT", 100.0 * (DT_d - DT_h) / DT_h, 159.0),
        ("DL", 100.0 * (DL_d - DL_h) / DL_h,  53.0),
    ]
    for name, pct, ref in checks:
        diff   = abs(pct - ref)
        status = "PASS" if diff <= tol_pct else "FAIL"
        if status == "FAIL":
            ok = False
        print(
            f"  {status}  {name}: {pct:+.1f} %  "
            f"(paper: {ref:+.1f} %,  |diff|: {diff:.1f} pp,  tol: {tol_pct:.0f} pp)"
        )
    return ok


# ---------------------------------------------------------------------------
# Command-line interface
# ---------------------------------------------------------------------------

def _build_parser():
    p = argparse.ArgumentParser(
        description="Reproduce Figure 2 of Boudreau et al. MRM 2012.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument(
        "files",
        nargs="*",
        metavar="FILE",
        help=(
            "Eight simulation output files, one per b-value "
            "(healthy geometry).  Shorthand for --healthy."
        ),
    )
    p.add_argument(
        "--healthy",
        nargs=8,
        metavar="FILE",
        help="Eight output files for the healthy geometry.",
    )
    p.add_argument(
        "--diseased",
        nargs=8,
        metavar="FILE",
        help="Eight output files for the diseased geometry.",
    )
    p.add_argument(
        "--b-values",
        nargs=8,
        type=float,
        default=[11, 22, 33, 44, 55, 66, 77, 88],
        metavar="B",
        help="b-values in s/cm² (default: 11 22 33 44 55 66 77 88).",
    )
    p.add_argument(
        "--n-angles",
        type=int,
        default=30,
        help="Number of gradient orientations (default: 30).",
    )
    p.add_argument(
        "--check",
        action="store_true",
        help=(
            "Compare fitted DL/DT against the paper's headline numbers. "
            "Requires both --healthy and --diseased."
        ),
    )
    return p


def main(argv=None):
    parser = _build_parser()
    args   = parser.parse_args(argv)

    b_values  = np.asarray(args.b_values)
    gradients = gradients_from_b(b_values)

    print("Gradient amplitudes (T/m) for the given b-values (s/cm²):")
    print(f"  {'b (s/cm²)':>12}  {'Gamp (T/m)':>18}")
    for bi, gi in zip(b_values, gradients):
        print(f"  {bi:12.1f}  {gi:18.12f}")
    print()

    healthy_files  = args.healthy or (args.files if args.files else None)
    diseased_files = args.diseased

    if healthy_files is None:
        print(
            "No simulation files supplied — showing gradient amplitudes only.\n"
            "Run the C simulation with the amplitudes above, then re-run:\n"
            f"    python {sys.argv[0]} b1.txt b2.txt b3.txt b4.txt "
            "b5.txt b6.txt b7.txt b8.txt"
        )
        return 0

    print("Fitting healthy geometry …")
    DL_h, DT_h, _ = analyze(healthy_files, b_values, args.n_angles)
    print(f"  DL = {DL_h:.6f} cm²/s")
    print(f"  DT = {DT_h:.6f} cm²/s")
    print(f"  (DL + 2·DT) / 3 = {(DL_h + 2*DT_h)/3:.6f} cm²/s")
    print()

    if diseased_files is not None:
        print("Fitting diseased geometry …")
        DL_d, DT_d, _ = analyze(diseased_files, b_values, args.n_angles)
        print(f"  DL = {DL_d:.6f} cm²/s")
        print(f"  DT = {DT_d:.6f} cm²/s")
        print(f"  (DL + 2·DT) / 3 = {(DL_d + 2*DT_d)/3:.6f} cm²/s")
        print()

        if args.check:
            print("Checking against paper headline numbers (Figure 2):")
            ok = check_paper_results(DL_h, DT_h, DL_d, DT_d)
            print()
            return 0 if ok else 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
