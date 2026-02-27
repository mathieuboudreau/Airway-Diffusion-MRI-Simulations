/*
 * bloch_torrey.c — Bloch-Torrey finite-difference diffusion MRI simulator
 *
 * Solves the Bloch-Torrey equation for hyperpolarised gas (Xe-129) diffusion
 * inside a budded-cylinder airway model using an explicit Euler scheme with
 * dimensional operator splitting.
 *
 * Coordinate convention (refactored repo):
 *   x (ii), y (jj) — radial cross-section of the cylinder
 *   z (kk)          — cylinder long axis  (periodic BC)
 *
 * The gradient is applied at angle θ relative to the cylinder axis:
 *   Gx = Gamp * sin(θ)   (radial)
 *   Gz = Gamp * cos(θ)   (axial)
 *
 * Gradient waveform: bipolar trapezoidal PGSE (2 lobes, each with
 *   rise / flat-top / fall phases).
 */

#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>

#include "bloch_torrey.h"

/* ------------------------------------------------------------------ */
/* Memory helpers                                                       */
/* ------------------------------------------------------------------ */

double ***alloc_3d_double(int xdim, int ydim, int zdim)
{
    double ***arr = malloc(xdim * sizeof(double **));
    for (int x = 0; x < xdim; x++) {
        arr[x] = malloc(ydim * sizeof(double *));
        for (int y = 0; y < ydim; y++)
            arr[x][y] = calloc(zdim, sizeof(double));
    }
    return arr;
}

void free_3d_double(double ***arr, int xdim, int ydim)
{
    for (int x = 0; x < xdim; x++) {
        for (int y = 0; y < ydim; y++)
            free(arr[x][y]);
        free(arr[x]);
    }
    free(arr);
}

float complex ***alloc_3d_complexf(int xdim, int ydim, int zdim)
{
    float complex ***arr = malloc(xdim * sizeof(float complex **));
    for (int x = 0; x < xdim; x++) {
        arr[x] = malloc(ydim * sizeof(float complex *));
        for (int y = 0; y < ydim; y++)
            arr[x][y] = calloc(zdim, sizeof(float complex));
    }
    return arr;
}

void free_3d_complexf(float complex ***arr, int xdim, int ydim)
{
    for (int x = 0; x < xdim; x++) {
        for (int y = 0; y < ydim; y++)
            free(arr[x][y]);
        free(arr[x]);
    }
    free(arr);
}

float complex **alloc_2d_complexf(int xdim, int zdim)
{
    float complex **arr = malloc(xdim * sizeof(float complex *));
    for (int x = 0; x < xdim; x++)
        arr[x] = calloc(zdim, sizeof(float complex));
    return arr;
}

void free_2d_complexf(float complex **arr, int xdim)
{
    for (int x = 0; x < xdim; x++)
        free(arr[x]);
    free(arr);
}

/* ------------------------------------------------------------------ */
/* Main solver                                                          */
/* ------------------------------------------------------------------ */

void run_bloch_torrey(const struct Volume               *vol,
                      const struct BuddedCylinderParams *geom,
                      const struct SimulationParams     *sim,
                      double                             angle,
                      int                                angle_idx,
                      FILE                              *fp)
{
    const int xdim = vol->xdim;
    const int ydim = vol->ydim;
    const int zdim = vol->zdim;

    const double dx = geom->dx;
    const double dy = geom->dy;
    const double dz = geom->dz;

    const double DHe      = sim->DHe;
    const double gamma    = sim->gamma;
    const double bigdelta = sim->bigdelta;
    const double risetime = sim->risetime;
    const double Gamp     = sim->Gamp;

    /* Gradient components for this angle */
    const double Gxamp = Gamp * sin(angle);   /* radial */
    const double Gzamp = Gamp * cos(angle);   /* axial  */

    /* ---- Time discretisation ---- */
    /* Stability: dt < dz²/(2*D) per direction; use safety factor 0.2 */
    const double dt  = 0.2 * dz * dz / DHe;
    const int    time_steps = (int)(bigdelta / dt);   /* steps per lobe  */
    const int    Nt  = 2 * time_steps;                /* total timesteps */
    const int    Nt1 = (int)(risetime * time_steps / bigdelta); /* rise/fall */
    const int    Nt2 = (int)((bigdelta - 2.0 * risetime) * time_steps / bigdelta); /* flat */

    const double slewrate  = Gamp / (double)Nt1;
    const double slewratex = slewrate * sin(angle);
    const double slewratez = slewrate * cos(angle);

    /* Diffusion couplings (unitless, absorbed into FD stencil) */
    const double kxd = dt / (dx * dx);
    const double kyd = dt / (dy * dy);
    const double kzd = dt / (dz * dz);

    /* Pre-multiplied imaginary factor: igammadt = -i*gamma*dt */
    const float complex igammadt = -(float complex)(gamma * dt) * I;

    /* Periodic BC active range along z (cylinder axis) */
    const int k_start = 1;
    const int k_end   = zdim - 2;

    /* ---- Position arrays (1-indexed style to match physics convention) ---- */
    /* X[i] = (i+1)*dx, Z[k] = (k+1)*dz  →  positions start at one step      */
    double *X = malloc(xdim * sizeof(double));
    double *Z = malloc(zdim * sizeof(double));
    for (int i = 0; i < xdim; i++) X[i] = (i + 1) * dx;
    for (int k = 0; k < zdim; k++) Z[k] = (k + 1) * dz;

    /* ---- Allocate simulation arrays ---- */
    double        ***Ddist      = alloc_3d_double  (xdim, ydim, zdim);
    float complex ***M1         = alloc_3d_complexf(xdim, ydim, zdim);
    float complex ***M2         = alloc_3d_complexf(xdim, ydim, zdim);
    float complex ***Mp         = alloc_3d_complexf(xdim, ydim, zdim);
    float complex **phasechange = alloc_2d_complexf(xdim, zdim);

    /* ---- Initialise: Ddist and M2 from geometry mask ---- */
    for (int ii = 0; ii < xdim; ii++) {
        for (int jj = 0; jj < ydim; jj++) {
            for (int kk = 0; kk < zdim; kk++) {
                int mask = vol->array[ii][jj][kk];
                Ddist[ii][jj][kk] = DHe * mask;
                M2[ii][jj][kk]    = (float complex)mask;
            }
        }
    }

    /* ---- Time loop ---- */
    double GBZ = 0.0;  /* cumulative gradient integral for periodic-BC phase */

    for (int t = 1; t <= Nt; t++) {

        /* -- Determine gradient amplitude for this timestep -- */
        double Gx, Gz;

        if (t <= Nt1) {
            /* First lobe: rise */
            Gx  = slewratex * t;
            Gz  = slewratez * t;
            GBZ += slewratez * t;
        } else if (t <= Nt1 + Nt2) {
            /* First lobe: flat top */
            Gx  = Gxamp;
            Gz  = Gzamp;
            GBZ += Gzamp;
        } else if (t <= time_steps) {
            /* First lobe: fall */
            Gx  = slewratex * (time_steps - t);
            Gz  = slewratez * (time_steps - t);
            GBZ += slewratez * (time_steps - t);
        } else if (t <= time_steps + Nt1) {
            /* Second lobe: rise (negative) */
            Gx  = -slewratex * (t - time_steps);
            Gz  = -slewratez * (t - time_steps);
            GBZ -= slewratez * (t - time_steps);
        } else if (t <= time_steps + Nt1 + Nt2) {
            /* Second lobe: flat top (negative) */
            Gx  = -Gxamp;
            Gz  = -Gzamp;
            GBZ -= Gzamp;
        } else {
            /* Second lobe: fall (negative) */
            Gx  = -slewratex * (Nt - t);
            Gz  = -slewratez * (Nt - t);
            GBZ -= slewratez * (Nt - t);
        }

        /* -- Phase rotation: build phasechange[ii][kk] then apply to M1 -- */
        for (int ii = 1; ii < xdim; ii++)
            for (int kk = k_start; kk <= k_end; kk++)
                phasechange[ii][kk] = cexpf(igammadt * (float complex)(Gx * X[ii] + Gz * Z[kk]));

        for (int ii = 1; ii < xdim; ii++)
            for (int jj = 1; jj < ydim - 1; jj++)
                for (int kk = k_start; kk <= k_end; kk++)
                    M1[ii][jj][kk] = M2[ii][jj][kk] * phasechange[ii][kk];

        /* -- Operator-split diffusion step: y → z (periodic) → x -- */
        float complex Signal = 0.0f + 0.0f * I;

        for (int ii = 1; ii < xdim - 1; ii++) {
            for (int jj = 1; jj < ydim - 1; jj++) {
                for (int kk = k_start; kk <= k_end; kk++) {

                    /* Step 1: y-direction (radial, natural zero-flux BC at walls) */
                    Mp[ii][jj][kk] = M1[ii][jj][kk]
                        + (float)kyd * (
                            (float)Ddist[ii][jj-1][kk] * (M1[ii][jj-1][kk] - M1[ii][jj][kk])
                          + (float)Ddist[ii][jj+1][kk] * (M1[ii][jj+1][kk] - M1[ii][jj][kk]));

                    /* Step 2: z-direction (axial, periodic BC) */
                    float complex left_nb, right_nb;

                    if (kk == k_start) {
                        /* Left neighbour wraps to k_end with phase correction */
                        float phase_corr_l = (float)(GBZ * (Z[k_start] - Z[k_end + 1]));
                        left_nb  = (float)Ddist[ii][jj][k_end]
                                 * (M1[ii][jj][k_end] * cexpf(igammadt * phase_corr_l)
                                    - M1[ii][jj][kk]);
                        right_nb = (float)Ddist[ii][jj][kk+1]
                                 * (M1[ii][jj][kk+1] - M1[ii][jj][kk]);
                    } else if (kk == k_end) {
                        /* Right neighbour wraps to k_start with phase correction */
                        left_nb  = (float)Ddist[ii][jj][kk-1]
                                 * (M1[ii][jj][kk-1] - M1[ii][jj][kk]);
                        float phase_corr_r = (float)(GBZ * (Z[k_end] - Z[k_start - 1]));
                        right_nb = (float)Ddist[ii][jj][k_start]
                                 * (M1[ii][jj][k_start] * cexpf(igammadt * phase_corr_r)
                                    - M1[ii][jj][kk]);
                    } else {
                        left_nb  = (float)Ddist[ii][jj][kk-1]
                                 * (M1[ii][jj][kk-1] - M1[ii][jj][kk]);
                        right_nb = (float)Ddist[ii][jj][kk+1]
                                 * (M1[ii][jj][kk+1] - M1[ii][jj][kk]);
                    }

                    M2[ii][jj][kk] = Mp[ii][jj][kk]
                        + (float)kzd * (left_nb + right_nb);

                    /* Step 3: x-direction (radial, natural zero-flux BC at walls) */
                    Mp[ii][jj][kk] = M2[ii][jj][kk]
                        + (float)kxd * (
                            (float)Ddist[ii-1][jj][kk] * (M1[ii-1][jj][kk] - M1[ii][jj][kk])
                          + (float)Ddist[ii+1][jj][kk] * (M1[ii+1][jj][kk] - M1[ii][jj][kk]));

                    /* Apply geometry mask (gas stays inside airway) */
                    M2[ii][jj][kk] = Mp[ii][jj][kk] * (float)vol->array[ii][jj][kk];

                    Signal += M2[ii][jj][kk];
                }
            }
        }

        fprintf(fp, "%d, %.8f, %d, %.8f, %.8f\n",
                angle_idx, angle, t, crealf(Signal), cabsf(Signal));
    }

    /* ---- Free all simulation arrays ---- */
    free_3d_double  (Ddist,       xdim, ydim);
    free_3d_complexf(M1,          xdim, ydim);
    free_3d_complexf(M2,          xdim, ydim);
    free_3d_complexf(Mp,          xdim, ydim);
    free_2d_complexf(phasechange, xdim);
    free(X);
    free(Z);
}
