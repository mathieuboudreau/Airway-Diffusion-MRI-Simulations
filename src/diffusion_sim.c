#define _USE_MATH_DEFINES
#include <stdio.h>
#include <string.h>
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
    if (argc < 3) {
        printf("Usage: diffusion_sim <output_file> <healthy|disease>\n");
        return 1;
    }

    const char *filename = argv[1];

    struct BuddedCylinderParams bcParams;
    if (strcmp(argv[2], "healthy") == 0) {
        bcParams = setupHealthyBuddedCylinder();
    } else if (strcmp(argv[2], "disease") == 0) {
        bcParams = setupDiseasedBuddedCylinder();
    } else {
        printf("Second argument must be \"healthy\" or \"disease\".\n");
        return 1;
    }

    /* Generate 3D airway geometry mask */
    struct Volume volume;
    volume.xdim = bcParams.xdim;
    volume.ydim = bcParams.ydim;
    volume.zdim = bcParams.zdim;
    allocvolume(&volume);
    generatevolume(&volume, &buddedcylinder, &bcParams);

    /* MRI simulation parameters (Xe-129 defaults) */
    struct SimulationParams simParams = setupDefaultSimulationParams();

    /* Open output CSV */
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Could not open output file: %s\n", filename);
        freevolume(&volume);
        return 1;
    }

    fprintf(fp, "angle_idx, angle_rad, t, real_signal, abs_signal\n");

    /* Simulate each gradient orientation */
    for (int n = 1; n <= simParams.num_angles; n++) {
        double angle = (double)n * M_PI / (double)simParams.num_angles;
        printf("  angle %2d/%d  (%.4f rad, %.1f deg)\n",
               n, simParams.num_angles, angle, angle * 180.0 / M_PI);
        run_bloch_torrey(&volume, &bcParams, &simParams, angle, n, fp);
    }

    fclose(fp);
    freevolume(&volume);

    printf("Done. Signal written to: %s\n", filename);
    return 0;
}
