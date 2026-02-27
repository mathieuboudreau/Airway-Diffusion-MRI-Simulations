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

#endif
