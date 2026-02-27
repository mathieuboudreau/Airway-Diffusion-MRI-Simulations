/*
 * compare_2011.c — run the Bloch-Torrey solver with the exact 2011 parameters
 * (bigdelta=5ms, Gamp=0.00964813...) for gradient angle n=1 and print the
 * first 500 timestep magnitudes so they can be compared against the reference
 * output produced by the original monolithic code.
 *
 * Usage: compare_2011 <output_csv>
 */
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "../src/morphology/generatevolume.h"
#include "../src/morphology/buddedcylinder.h"
#include "../src/params/default_buddedcylinders.h"
#include "../src/params/simulation_params.h"
#include "../src/simulation/bloch_torrey.h"

int main(int argc, char *argv[])
{
    const char *filename = (argc >= 2) ? argv[1] : "compare_2011_out.csv";

    struct BuddedCylinderParams bcParams = setupHealthyBuddedCylinder();

    struct Volume volume;
    volume.xdim = bcParams.xdim;
    volume.ydim = bcParams.ydim;
    volume.zdim = bcParams.zdim;
    allocvolume(&volume);
    generatevolume(&volume, &buddedcylinder, &bcParams);

    struct SimulationParams simParams = setup2011SimulationParams();
    /* Only run angle n=1 */
    simParams.num_angles = 1;

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Cannot open %s\n", filename);
        freevolume(&volume);
        return 1;
    }

    fprintf(fp, "angle_idx, angle_rad, t, real_signal, abs_signal\n");

    double angle = 1.0 * M_PI / 30.0;
    run_bloch_torrey(&volume, &bcParams, &simParams, angle, 1, fp);

    fclose(fp);
    freevolume(&volume);

    printf("Done. Output written to: %s\n", filename);
    return 0;
}
