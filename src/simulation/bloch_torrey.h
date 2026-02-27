#ifndef BLOCH_TORREY_H
#define BLOCH_TORREY_H

#include <complex.h>
#include <stdio.h>
#include "../morphology/generatevolume.h"
#include "../params/default_buddedcylinders.h"
#include "../params/simulation_params.h"

/*
 * Memory helpers — same malloc/free pattern as allocvolume/freevolume.
 * Exported so tests can exercise them independently.
 */
double        ***alloc_3d_double  (int xdim, int ydim, int zdim);
void           free_3d_double     (double ***arr, int xdim, int ydim);

float complex ***alloc_3d_complexf(int xdim, int ydim, int zdim);
void           free_3d_complexf   (float complex ***arr, int xdim, int ydim);

float complex **alloc_2d_complexf (int xdim, int zdim);
void           free_2d_complexf   (float complex **arr, int xdim);

/*
 * run_bloch_torrey — simulate one gradient angle using the Bloch-Torrey
 * finite-difference solver and write the signal time series to fp.
 *
 * Output columns: angle_idx, t, real_signal, abs_signal
 *
 * The cylinder axis is assumed to be along the z direction of vol (matching
 * the refactored generatevolume convention). Periodic boundary conditions are
 * applied at k = 1 and k = zdim-2.
 */
void run_bloch_torrey(const struct Volume            *vol,
                      const struct BuddedCylinderParams *geom,
                      const struct SimulationParams  *sim,
                      double                          angle,
                      int                             angle_idx,
                      FILE                           *fp);

#endif
