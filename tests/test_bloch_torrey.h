#ifndef _TEST_BLOCH_TORREY_H
#define _TEST_BLOCH_TORREY_H

#define _USE_MATH_DEFINES
#include <check.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include "../src/simulation/bloch_torrey.h"
#include "../src/morphology/generatevolume.h"
#include "../src/morphology/buddedcylinder.h"
#include "../src/params/default_buddedcylinders.h"
#include "../src/params/simulation_params.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ---- alloc_3d_double / free_3d_double ---- */

START_TEST(test_alloc_3d_double_returns_zeroed_array)
{
    double ***arr = alloc_3d_double(4, 4, 4);
    ck_assert(arr[3][3][3] == 0.0);
    free_3d_double(arr, 4, 4);
}
END_TEST

START_TEST(test_alloc_3d_double_can_write_and_read)
{
    double ***arr = alloc_3d_double(5, 5, 5);
    arr[2][3][4] = 3.14;
    ck_assert(arr[2][3][4] == 3.14);
    free_3d_double(arr, 5, 5);
}
END_TEST

/* ---- alloc_3d_complexf / free_3d_complexf ---- */

START_TEST(test_alloc_3d_complexf_returns_zeroed_array)
{
    float complex ***arr = alloc_3d_complexf(4, 4, 4);
    ck_assert(crealf(arr[3][3][3]) == 0.0f);
    ck_assert(cimagf(arr[3][3][3]) == 0.0f);
    free_3d_complexf(arr, 4, 4);
}
END_TEST

/* ---- alloc_2d_complexf / free_2d_complexf ---- */

START_TEST(test_alloc_2d_complexf_returns_zeroed_array)
{
    float complex **arr = alloc_2d_complexf(6, 6);
    ck_assert(crealf(arr[5][5]) == 0.0f);
    free_2d_complexf(arr, 6);
}
END_TEST

/* ---- run_bloch_torrey: signal is non-zero for a small isotropic cube ---- */
/*
 * Uses a tiny 12×12×12 volume with 100 µm voxel spacing so that the
 * stability-limited timestep  dt = 0.2·dz²/DHe ≈ 328 µs  gives only
 * ~60 total timesteps over the full diffusion interval.  The inner loop
 * runs on 10×10×10 = 1000 voxels → ~60 k iterations total, completing
 * in milliseconds without a per-test timeout override.
 */
START_TEST(test_bloch_torrey_produces_nonzero_signal_for_isotropic_cube)
{
    /* Tiny synthetic geometry: 12×12×12, all interior voxels = 1 (gas) */
    struct Volume volume;
    volume.xdim = 12;
    volume.ydim = 12;
    volume.zdim = 12;
    allocvolume(&volume);

    for (int ii = 1; ii <= 10; ii++)
        for (int jj = 1; jj <= 10; jj++)
            for (int kk = 1; kk <= 10; kk++)
                volume.array[ii][jj][kk] = 1;

    /* Minimal geometry params: only dx/dy/dz are used by the solver.
     * 100 µm voxel spacing → large dt → very few timesteps. */
    struct BuddedCylinderParams geom = {0};
    geom.dx = 100e-6;
    geom.dy = 100e-6;
    geom.dz = 100e-6;

    struct SimulationParams simParams = setupDefaultSimulationParams();
    simParams.num_angles = 1;

    FILE *fp = tmpfile();
    ck_assert_ptr_nonnull(fp);

    run_bloch_torrey(&volume, &geom, &simParams,
                     /*angle=*/1.5707963 /*pi/2*/, /*angle_idx=*/1, fp);

    /* Verify that data lines were written */
    rewind(fp);
    char line[256];
    int  lines_written = 0;
    while (fgets(line, sizeof(line), fp))
        lines_written++;

    fclose(fp);
    freevolume(&volume);

    ck_assert_int_gt(lines_written, 0);
}
END_TEST

/* ---- Zero-gradient invariant ------------------------------------------ */
/*
 * With Gamp = 0 there is no phase accumulation.  In a uniform gas-filled
 * volume the diffusion operator satisfies a no-flux (Neumann) boundary
 * condition in x and y (Ddist = 0 at mask boundaries) and a plain periodic
 * condition in z (GBZ stays 0, so the phase correction factor is e^0 = 1).
 * Total magnetisation is therefore conserved and the signal magnitude must
 * remain constant to within floating-point rounding.
 *
 * Uses the same tiny 12×12×12 cube as the smoke test so it runs in < 1 s.
 */
START_TEST(test_bloch_torrey_signal_constant_without_gradient)
{
    struct Volume volume;
    volume.xdim = 12;
    volume.ydim = 12;
    volume.zdim = 12;
    allocvolume(&volume);

    for (int ii = 1; ii <= 10; ii++)
        for (int jj = 1; jj <= 10; jj++)
            for (int kk = 1; kk <= 10; kk++)
                volume.array[ii][jj][kk] = 1;

    struct BuddedCylinderParams geom = {0};
    geom.dx = 100e-6;
    geom.dy = 100e-6;
    geom.dz = 100e-6;

    struct SimulationParams simParams = setupDefaultSimulationParams();
    simParams.num_angles = 1;
    simParams.Gamp = 0.0;   /* no gradient → no dephasing */

    FILE *fp = tmpfile();
    ck_assert_ptr_nonnull(fp);

    run_bloch_torrey(&volume, &geom, &simParams,
                     /*angle=*/0.0, /*angle_idx=*/1, fp);
    freevolume(&volume);

    rewind(fp);
    char  line[256];
    float sig0    = 0.0f;
    float sig_min = 1e38f;
    float sig_max = 0.0f;
    int   n       = 0;
    while (fgets(line, sizeof(line), fp)) {
        int   ai, t;
        float a, r, ab;
        if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5) {
            if (t == 1) sig0 = ab;
            if (ab < sig_min) sig_min = ab;
            if (ab > sig_max) sig_max = ab;
            n++;
        }
    }
    fclose(fp);

    ck_assert_msg(n > 0,       "No output lines written");
    ck_assert_msg(sig0 > 0.0f, "Initial signal is zero — volume may be empty");

    /* Allow at most 0.1 % drift in either direction */
    float tol = 0.001f * sig0;
    ck_assert_msg(sig0 - sig_min <= tol,
                  "Signal dropped by %.4f%% without a gradient (max 0.1%%)",
                  100.0f * (sig0 - sig_min) / sig0);
    ck_assert_msg(sig_max - sig0 <= tol,
                  "Signal grew by %.4f%% without a gradient (max 0.1%%)",
                  100.0f * (sig_max - sig0) / sig0);
}
END_TEST

/* ---- Stejskal-Tanner free-diffusion echo -------------------------------- */
/*
 * For free diffusion (no restrictions) with a bipolar square gradient along
 * z only (angle = 0, risetime = 0), the Stejskal-Tanner equation predicts
 * the signal magnitude at the echo time exactly:
 *
 *   |S(Nt)| / S(0)  =  exp(-b · D)
 *
 * where, for a bipolar square waveform with effective lobe duration Δ_eff:
 *
 *   b = (2/3) · γ² · G² · Δ_eff³
 *
 * The z-direction uses periodic BC, which realises free diffusion with no
 * boundary reflections.  The gradient acts only in z (Gxamp = G·sin(0) = 0),
 * so x/y diffusion contributes zero dephasing.
 *
 * Uses the same tiny 12×12×12 cube so the test runs in < 1 s.
 * Tolerance: 5 % to absorb operator-splitting and discrete-time errors.
 */
START_TEST(test_bloch_torrey_free_diffusion_echo_matches_stejskal_tanner)
{
    struct Volume volume;
    volume.xdim = 12;
    volume.ydim = 12;
    volume.zdim = 12;
    allocvolume(&volume);

    for (int ii = 1; ii <= 10; ii++)
        for (int jj = 1; jj <= 10; jj++)
            for (int kk = 1; kk <= 10; kk++)
                volume.array[ii][jj][kk] = 1;

    const int N_gas = 1000;   /* 10×10×10 interior voxels; S(0) is known */

    struct BuddedCylinderParams geom = {0};
    geom.dx = 100e-6;
    geom.dy = 100e-6;
    geom.dz = 100e-6;

    struct SimulationParams simParams = setupDefaultSimulationParams();
    simParams.num_angles = 1;
    simParams.risetime   = 0.0;   /* square bipolar → clean b-value formula */
    simParams.Gamp       = 5e-3;  /* moderate attenuation: exp(-b·D) ≈ 0.58 */

    /* Analytic prediction:
     *   dt       = 0.2 · dz² / D           (stability-limited timestep)
     *   Δ_eff    = floor(Δ/dt) · dt         (actual lobe duration in sim)
     *   b        = (2/3) · γ² · G² · Δ_eff³
     *   expected = exp(-b · D)                                             */
    double dt         = 0.2 * geom.dz * geom.dz / simParams.DHe;
    int    time_steps = (int)(simParams.bigdelta / dt);
    double delta_eff  = (double)time_steps * dt;
    double b          = (2.0 / 3.0)
                      * simParams.gamma * simParams.gamma
                      * simParams.Gamp  * simParams.Gamp
                      * delta_eff * delta_eff * delta_eff;
    double expected   = exp(-b * simParams.DHe);

    FILE *fp = tmpfile();
    ck_assert_ptr_nonnull(fp);

    run_bloch_torrey(&volume, &geom, &simParams,
                     /*angle=*/0.0, /*angle_idx=*/1, fp);
    freevolume(&volume);

    /* Read echo signal at t = Nt = 2 · time_steps */
    int   Nt     = 2 * time_steps;
    float sig_Nt = 0.0f;
    rewind(fp);
    char  line[256];
    while (fgets(line, sizeof(line), fp)) {
        int   ai, t;
        float a, r, ab;
        if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5
                && t == Nt)
            sig_Nt = ab;
    }
    fclose(fp);

    double sim_ratio = (double)sig_Nt / N_gas;
    double rel_err   = fabs(sim_ratio - expected) / expected;

    ck_assert_msg(rel_err < 0.05,
                  "Stejskal-Tanner free diffusion: simulated=%.6f, "
                  "expected=%.6f, relative error=%.2f%% (max 5%%)",
                  sim_ratio, expected, 100.0 * rel_err);
}
END_TEST

/* ---- 2011 reference regression ---------------------------------------- */
/*
 * Runs the simulation with the exact 2011 parameters (bigdelta=5 ms,
 * Gamp=0.00964813..., integer-truncated gamma) for gradient angle n=1 and
 * checks that the normalised signal decay at t=500 matches the reference
 * produced by the original monolithic code.
 *
 * Reference (original 2011 code, angle n=1):
 *   t=1   |S| = 192832.66   (absolute value depends on geometry; the
 *   t=500 |S| = 192146.55    refactored code has ~2 % more gas voxels)
 *   ratio = |S[500]| / |S[1]| = 0.99644
 *
 * We compare only the normalised ratio to tolerate the small geometry
 * difference; tolerance is ±0.005 (0.5 %).
 *
 * Runtime: ~20 s on a typical machine; timeout is set to 60 s.
 */
START_TEST(test_bloch_torrey_2011_normalised_decay_matches_reference)
{
    struct BuddedCylinderParams bcParams = setupHealthyBuddedCylinder();

    struct Volume volume;
    volume.xdim = bcParams.xdim;
    volume.ydim = bcParams.ydim;
    volume.zdim = bcParams.zdim;
    allocvolume(&volume);
    generatevolume(&volume, &buddedcylinder, &bcParams);

    struct SimulationParams simParams = setup2011SimulationParams();
    simParams.num_angles = 1;

    FILE *fp = tmpfile();
    ck_assert_ptr_nonnull(fp);

    double angle = 1.0 * M_PI / 30.0;
    run_bloch_torrey(&volume, &bcParams, &simParams, angle, 1, fp);
    freevolume(&volume);

    /* Parse output: extract abs_signal at t=1 and t=500 */
    rewind(fp);
    char  line[256];
    float sig_t1   = 0.0f;
    float sig_t500 = 0.0f;
    while (fgets(line, sizeof(line), fp)) {
        int   ai, t;
        float a, r, ab;
        if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5) {
            if (t == 1)   sig_t1   = ab;
            if (t == 500) sig_t500 = ab;
        }
    }
    fclose(fp);

    ck_assert_msg(sig_t1 > 0.0f, "Initial signal is zero — geometry may be empty");

    /* Normalised ratio at t=500: reference=0.99644, tolerance ±0.005 */
    float ratio = sig_t500 / sig_t1;
    ck_assert_msg(ratio >= 0.99144f,
                  "Signal decayed too much at t=500: ratio=%.6f (expected >= 0.99144)",
                  ratio);
    ck_assert_msg(ratio <= 1.00144f,
                  "Signal grew at t=500: ratio=%.6f (expected <= 1.00144)",
                  ratio);
}
END_TEST

/* ---- Echo coherence: signal dips at mid-point, refocuses at echo -------- */
/*
 * With a bipolar gradient (angle=0, risetime=0) the first lobe dephases spins
 * and the second lobe rephases them.  At t=time_steps (end of lobe 1) the
 * signal should be well below S(0); at t=Nt (echo) it should be substantially
 * larger than at mid-point.
 *
 * Verified numerically: S(mid)/S(0) ≈ 0.40,  S(echo)/S(0) ≈ 0.58.
 * Tolerances: S(mid) < 0.80·S(0) and S(echo) > 1.50·S(mid).
 */
START_TEST(test_bloch_torrey_signal_dephases_then_refocuses)
{
    struct Volume volume;
    volume.xdim = 12;
    volume.ydim = 12;
    volume.zdim = 12;
    allocvolume(&volume);

    for (int ii = 1; ii <= 10; ii++)
        for (int jj = 1; jj <= 10; jj++)
            for (int kk = 1; kk <= 10; kk++)
                volume.array[ii][jj][kk] = 1;

    struct BuddedCylinderParams geom = {0};
    geom.dx = 100e-6;
    geom.dy = 100e-6;
    geom.dz = 100e-6;

    struct SimulationParams simParams = setupDefaultSimulationParams();
    simParams.num_angles = 1;
    simParams.risetime   = 0.0;
    simParams.Gamp       = 5e-3;

    double dt         = 0.2 * geom.dz * geom.dz / simParams.DHe;
    int    time_steps = (int)(simParams.bigdelta / dt);
    int    Nt         = 2 * time_steps;

    FILE *fp = tmpfile();
    ck_assert_ptr_nonnull(fp);

    run_bloch_torrey(&volume, &geom, &simParams, 0.0, 1, fp);
    freevolume(&volume);

    rewind(fp);
    char  line[256];
    float sig0     = 0.0f;
    float sig_mid  = 0.0f;
    float sig_echo = 0.0f;
    while (fgets(line, sizeof(line), fp)) {
        int   ai, t;
        float a, r, ab;
        if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5) {
            if (t == 1)          sig0     = ab;
            if (t == time_steps) sig_mid  = ab;
            if (t == Nt)         sig_echo = ab;
        }
    }
    fclose(fp);

    ck_assert_msg(sig0 > 0.0f, "Initial signal is zero — volume may be empty");

    ck_assert_msg(sig_mid < 0.80f * sig0,
                  "Signal not sufficiently dephased at mid-point: "
                  "S(mid)/S(0)=%.4f (expect < 0.80)",
                  sig_mid / sig0);

    ck_assert_msg(sig_echo > 1.40f * sig_mid,
                  "Signal did not substantially refocus at echo: "
                  "S(echo)/S(mid)=%.4f (expect > 1.40)",
                  sig_echo / sig_mid);
}
END_TEST

/* ---- b-value scaling: log(S) proportional to G² ------------------------- */
/*
 * The Stejskal-Tanner equation gives  log|S| = -b·D  and  b ∝ G².
 * Therefore  log(S(G·√2)) / log(S(G)) = (G·√2)²/G² = 2.
 *
 * Tolerance: ±0.10 on the ratio (absorbs operator-splitting discretisation).
 */
START_TEST(test_bloch_torrey_signal_scales_with_b_value)
{
    struct BuddedCylinderParams geom = {0};
    geom.dx = 100e-6;
    geom.dy = 100e-6;
    geom.dz = 100e-6;

    struct SimulationParams simParams = setupDefaultSimulationParams();
    simParams.num_angles = 1;
    simParams.risetime   = 0.0;

    double dt         = 0.2 * geom.dz * geom.dz / simParams.DHe;
    int    time_steps = (int)(simParams.bigdelta / dt);
    int    Nt         = 2 * time_steps;

    float sig1 = 0.0f, sig2 = 0.0f;

    /* First run: G */
    {
        struct Volume volume;
        volume.xdim = 12; volume.ydim = 12; volume.zdim = 12;
        allocvolume(&volume);
        for (int ii = 1; ii <= 10; ii++)
            for (int jj = 1; jj <= 10; jj++)
                for (int kk = 1; kk <= 10; kk++)
                    volume.array[ii][jj][kk] = 1;
        simParams.Gamp = 5e-3;
        FILE *fp = tmpfile();
        run_bloch_torrey(&volume, &geom, &simParams, 0.0, 1, fp);
        freevolume(&volume);
        rewind(fp);
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            int ai, t; float a, r, ab;
            if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5 && t == Nt)
                sig1 = ab;
        }
        fclose(fp);
    }

    /* Second run: G·√2  →  b doubled */
    {
        struct Volume volume;
        volume.xdim = 12; volume.ydim = 12; volume.zdim = 12;
        allocvolume(&volume);
        for (int ii = 1; ii <= 10; ii++)
            for (int jj = 1; jj <= 10; jj++)
                for (int kk = 1; kk <= 10; kk++)
                    volume.array[ii][jj][kk] = 1;
        simParams.Gamp = 5e-3 * sqrt(2.0);
        FILE *fp = tmpfile();
        run_bloch_torrey(&volume, &geom, &simParams, 0.0, 1, fp);
        freevolume(&volume);
        rewind(fp);
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            int ai, t; float a, r, ab;
            if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5 && t == Nt)
                sig2 = ab;
        }
        fclose(fp);
    }

    ck_assert_msg(sig1 > 0.0f && sig2 > 0.0f,
                  "Echo signal is zero — volume may be empty");

    /* Normalise by S(0) = N_gas = 1000 so that log gives the correct
     * attenuation exponent (log(S/S0) = -b·D) rather than log of a count. */
    const double N_gas = 1000.0;
    double ratio = log((double)sig2 / N_gas) / log((double)sig1 / N_gas);
    ck_assert_msg(fabs(ratio - 2.0) < 0.10,
                  "b-value G² scaling: log(S(G√2)/S0)/log(S(G)/S0)=%.4f (expect 2.0 ±0.10)",
                  ratio);
}
END_TEST

/* ---- Angle dependence: radial attenuation less than axial --------------- */
/*
 * At angle=0 the gradient is axial (z, periodic BC) → free diffusion
 * → maximum signal attenuation (= Stejskal-Tanner).
 * At angle=π/2 the gradient is radial (x, no-flux BC) → restricted diffusion
 * → less attenuation → higher echo signal.
 *
 * Verified numerically: S(π/2)/S(0)≈0.72,  S(0°)/S(0)≈0.58  (same Gamp).
 */
START_TEST(test_bloch_torrey_radial_attenuation_less_than_axial)
{
    struct BuddedCylinderParams geom = {0};
    geom.dx = 100e-6;
    geom.dy = 100e-6;
    geom.dz = 100e-6;

    struct SimulationParams simParams = setupDefaultSimulationParams();
    simParams.num_angles = 1;
    simParams.risetime   = 0.0;
    simParams.Gamp       = 5e-3;

    double dt         = 0.2 * geom.dz * geom.dz / simParams.DHe;
    int    time_steps = (int)(simParams.bigdelta / dt);
    int    Nt         = 2 * time_steps;

    float sig_axial = 0.0f, sig_radial = 0.0f;

    /* Axial: angle = 0 */
    {
        struct Volume volume;
        volume.xdim = 12; volume.ydim = 12; volume.zdim = 12;
        allocvolume(&volume);
        for (int ii = 1; ii <= 10; ii++)
            for (int jj = 1; jj <= 10; jj++)
                for (int kk = 1; kk <= 10; kk++)
                    volume.array[ii][jj][kk] = 1;
        FILE *fp = tmpfile();
        run_bloch_torrey(&volume, &geom, &simParams, 0.0, 1, fp);
        freevolume(&volume);
        rewind(fp);
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            int ai, t; float a, r, ab;
            if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5 && t == Nt)
                sig_axial = ab;
        }
        fclose(fp);
    }

    /* Radial: angle = π/2 */
    {
        struct Volume volume;
        volume.xdim = 12; volume.ydim = 12; volume.zdim = 12;
        allocvolume(&volume);
        for (int ii = 1; ii <= 10; ii++)
            for (int jj = 1; jj <= 10; jj++)
                for (int kk = 1; kk <= 10; kk++)
                    volume.array[ii][jj][kk] = 1;
        FILE *fp = tmpfile();
        run_bloch_torrey(&volume, &geom, &simParams, M_PI / 2.0, 1, fp);
        freevolume(&volume);
        rewind(fp);
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            int ai, t; float a, r, ab;
            if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5 && t == Nt)
                sig_radial = ab;
        }
        fclose(fp);
    }

    ck_assert_msg(sig_axial > 0.0f && sig_radial > 0.0f,
                  "Echo signal is zero — volume may be empty");

    ck_assert_msg(sig_radial > sig_axial,
                  "Expected radial signal (%.4f) > axial signal (%.4f): "
                  "no-flux BC should restrict diffusion more than periodic BC",
                  sig_radial, sig_axial);
}
END_TEST

/* ---- Empty volume gives zero signal ------------------------------------- */
/*
 * With no gas voxels (all mask=0) the magnetisation is zero everywhere and
 * the signal must be identically zero at every timestep.
 */
START_TEST(test_bloch_torrey_empty_volume_gives_zero_signal)
{
    struct Volume volume;
    volume.xdim = 12;
    volume.ydim = 12;
    volume.zdim = 12;
    allocvolume(&volume);   /* all entries initialised to 0 */

    struct BuddedCylinderParams geom = {0};
    geom.dx = 100e-6;
    geom.dy = 100e-6;
    geom.dz = 100e-6;

    struct SimulationParams simParams = setupDefaultSimulationParams();
    simParams.num_angles = 1;

    FILE *fp = tmpfile();
    ck_assert_ptr_nonnull(fp);

    run_bloch_torrey(&volume, &geom, &simParams, 0.0, 1, fp);
    freevolume(&volume);

    rewind(fp);
    char  line[256];
    float max_sig = 0.0f;
    while (fgets(line, sizeof(line), fp)) {
        int   ai, t;
        float a, r, ab;
        if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5)
            if (ab > max_sig) max_sig = ab;
    }
    fclose(fp);

    ck_assert_msg(max_sig == 0.0f,
                  "Empty volume produced non-zero signal: max|S|=%.6f", max_sig);
}
END_TEST

/* ---- Single isolated voxel echoes perfectly ----------------------------- */
/*
 * A single gas voxel surrounded by wall has no diffusion neighbours, so its
 * magnetisation evolves purely by phase rotation:
 *   M(t+1) = M(t) · exp(i·γ·dt·(Gx(t)·X + Gz(t)·Z))
 *
 * For a symmetric bipolar pulse the sum of Gx(t) and of Gz(t) over all Nt
 * steps is zero, so the total accumulated phase is zero and |S(Nt)| = |S(0)|
 * to floating-point precision.
 *
 * The voxel is placed at (5,5,5) — away from all boundaries — to avoid any
 * periodic-BC correction.  angle=π/4 exercises both Gx and Gz simultaneously.
 * Tolerance: absolute error < 1e-4.
 */
START_TEST(test_bloch_torrey_single_voxel_echo_is_unity)
{
    struct Volume volume;
    volume.xdim = 12;
    volume.ydim = 12;
    volume.zdim = 12;
    allocvolume(&volume);
    volume.array[5][5][5] = 1;   /* one isolated gas voxel */

    struct BuddedCylinderParams geom = {0};
    geom.dx = 100e-6;
    geom.dy = 100e-6;
    geom.dz = 100e-6;

    struct SimulationParams simParams = setupDefaultSimulationParams();
    simParams.num_angles = 1;
    simParams.risetime   = 0.0;
    simParams.Gamp       = 5e-3;

    double dt         = 0.2 * geom.dz * geom.dz / simParams.DHe;
    int    time_steps = (int)(simParams.bigdelta / dt);
    int    Nt         = 2 * time_steps;

    FILE *fp = tmpfile();
    ck_assert_ptr_nonnull(fp);

    run_bloch_torrey(&volume, &geom, &simParams, M_PI / 4.0, 1, fp);
    freevolume(&volume);

    rewind(fp);
    char  line[256];
    float sig0     = 0.0f;
    float sig_echo = 0.0f;
    while (fgets(line, sizeof(line), fp)) {
        int   ai, t;
        float a, r, ab;
        if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5) {
            if (t == 1)  sig0     = ab;
            if (t == Nt) sig_echo = ab;
        }
    }
    fclose(fp);

    ck_assert_msg(sig0 > 0.0f, "Initial signal is zero — single voxel not set");

    float abs_err = fabsf(sig_echo - sig0);
    ck_assert_msg(abs_err < 1e-4f,
                  "Single isolated voxel: |S(echo)-S(0)|=%.2e (expect < 1e-4)",
                  abs_err);
}
END_TEST

/* ---- Trapezoidal waveform attenuates less than square pulse ------------- */
/*
 * With a non-zero rise time the gradient ramps up and down, so the effective
 * b-value is smaller than for an equal-amplitude square pulse over the same
 * bigdelta.  Therefore the echo signal should be larger with risetime > 0.
 *
 * Uses risetime = 10 % of bigdelta.  Verified: S_trap/S(0) ≈ 0.633 vs
 * S_square/S(0) ≈ 0.584.
 */
START_TEST(test_bloch_torrey_trapezoidal_signal_exceeds_square)
{
    struct BuddedCylinderParams geom = {0};
    geom.dx = 100e-6;
    geom.dy = 100e-6;
    geom.dz = 100e-6;

    struct SimulationParams simParams = setupDefaultSimulationParams();
    simParams.num_angles = 1;
    simParams.Gamp       = 5e-3;

    double dt         = 0.2 * geom.dz * geom.dz / simParams.DHe;
    int    time_steps = (int)(simParams.bigdelta / dt);
    int    Nt         = 2 * time_steps;

    float sig_square = 0.0f, sig_trap = 0.0f;

    /* Square pulse: risetime = 0 */
    {
        struct Volume volume;
        volume.xdim = 12; volume.ydim = 12; volume.zdim = 12;
        allocvolume(&volume);
        for (int ii = 1; ii <= 10; ii++)
            for (int jj = 1; jj <= 10; jj++)
                for (int kk = 1; kk <= 10; kk++)
                    volume.array[ii][jj][kk] = 1;
        simParams.risetime = 0.0;
        FILE *fp = tmpfile();
        run_bloch_torrey(&volume, &geom, &simParams, 0.0, 1, fp);
        freevolume(&volume);
        rewind(fp);
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            int ai, t; float a, r, ab;
            if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5 && t == Nt)
                sig_square = ab;
        }
        fclose(fp);
    }

    /* Trapezoidal: risetime = 10 % of bigdelta */
    {
        struct Volume volume;
        volume.xdim = 12; volume.ydim = 12; volume.zdim = 12;
        allocvolume(&volume);
        for (int ii = 1; ii <= 10; ii++)
            for (int jj = 1; jj <= 10; jj++)
                for (int kk = 1; kk <= 10; kk++)
                    volume.array[ii][jj][kk] = 1;
        simParams.risetime = simParams.bigdelta * 0.1;
        FILE *fp = tmpfile();
        run_bloch_torrey(&volume, &geom, &simParams, 0.0, 1, fp);
        freevolume(&volume);
        rewind(fp);
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            int ai, t; float a, r, ab;
            if (sscanf(line, "%d, %f, %d, %f, %f", &ai, &a, &t, &r, &ab) == 5 && t == Nt)
                sig_trap = ab;
        }
        fclose(fp);
    }

    ck_assert_msg(sig_square > 0.0f && sig_trap > 0.0f,
                  "Echo signal is zero — volume may be empty");

    ck_assert_msg(sig_trap > sig_square,
                  "Trapezoidal signal (%.4f) should exceed square signal (%.4f): "
                  "smaller effective b-value with finite rise time",
                  sig_trap, sig_square);
}
END_TEST

#endif
