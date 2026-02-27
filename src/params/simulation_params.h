#ifndef SIMULATION_PARAMS_H
#define SIMULATION_PARAMS_H

struct SimulationParams
{
    double DHe;        /* Free-air diffusion coefficient of Xe-129 (m^2/s) */
    double gamma;      /* Gyromagnetic ratio of Xe-129 (rad/s/T)          */
    double bigdelta;   /* Diffusion encoding time (s)                     */
    double risetime;   /* Gradient rise/fall time (s)                     */
    double Gamp;       /* Gradient amplitude (T/m)                        */
    int    num_angles; /* Number of gradient orientations to simulate     */
};

struct SimulationParams setupDefaultSimulationParams(void);

#endif
