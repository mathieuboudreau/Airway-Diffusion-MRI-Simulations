#define _USE_MATH_DEFINES
#include <math.h>
#include "simulation_params.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct SimulationParams setupDefaultSimulationParams(void)
{
    struct SimulationParams p;

    /* Xe-129 free-air diffusion coefficient (m^2/s) */
    p.DHe = 6.1e-6;

    /* Gyromagnetic ratio of Xe-129: gamma = 2*pi * (-11860390 Hz/T) */
    p.gamma = 2.0 * M_PI * (-11860390.0);

    /* Diffusion encoding time (s) */
    p.bigdelta = 10e-3;

    /* Gradient rise/fall time (s) */
    p.risetime = 1e-3;

    /* Gradient amplitude (T/m) — chosen to give ADC-sensitive b-value */
    p.Gamp = 0.0118363762739716;

    /* Number of gradient angles: 0 to pi in num_angles equal steps */
    p.num_angles = 30;

    return p;
}
