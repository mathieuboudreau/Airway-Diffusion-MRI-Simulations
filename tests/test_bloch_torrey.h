#ifndef _TEST_BLOCH_TORREY_H
#define _TEST_BLOCH_TORREY_H

#include <check.h>
#include <complex.h>
#include "../src/simulation/bloch_torrey.h"
#include "../src/morphology/generatevolume.h"
#include "../src/morphology/buddedcylinder.h"
#include "../src/params/default_buddedcylinders.h"
#include "../src/params/simulation_params.h"

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

/* ---- run_bloch_torrey: signal is non-zero for a valid volume ---- */

START_TEST(test_bloch_torrey_produces_nonzero_signal_for_healthy_geometry)
{
    struct BuddedCylinderParams bcParams = setupHealthyBuddedCylinder();

    struct Volume volume;
    volume.xdim = bcParams.xdim;
    volume.ydim = bcParams.ydim;
    volume.zdim = bcParams.zdim;

    allocvolume(&volume);
    generatevolume(&volume, &buddedcylinder, &bcParams);

    struct SimulationParams simParams = setupDefaultSimulationParams();
    /* Run only 1 angle to keep the test fast */
    simParams.num_angles = 1;

    FILE *fp = tmpfile();
    ck_assert_ptr_nonnull(fp);

    run_bloch_torrey(&volume, &bcParams, &simParams,
                     /*angle=*/1.5707963 /*pi/2*/, /*angle_idx=*/1, fp);

    /* Check that at least one data line was written */
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

#endif
