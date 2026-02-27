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

struct SimulationParams setup2011SimulationParams(void)
{
    struct SimulationParams p;

    /* Xe-129, matching the 2011 grad-student version of the simulation.
     * He = -11860390 Hz/T, gammaHe = (int)(2*pi*He) = -74502697 rad/s/T
     * (the original code truncated to int; the tiny difference from the
     * precise double value is negligible but we replicate it for fidelity). */
    p.DHe      = 6.1e-6;
    p.gamma    = (double)((int)(2.0 * M_PI * (-11860390.0)));  /* = -74502697 */
    p.bigdelta = 5e-3;
    p.risetime = 1e-3;
    p.Gamp     = 0.00964813557811053;
    p.num_angles = 30;

    return p;
}
