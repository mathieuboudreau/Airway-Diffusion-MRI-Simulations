#ifndef DEFAULT_BUDDEDCYLINDERS_H
#define DEFAULT_BUDDEDCYLINDERS_H

struct BuddedCylinderParams
{
    int     xdim, ydim, zdim;

    double  dx,   dy,   dz;


    int     cyl_radius_unit_steps,    cyl_length_unit_steps;
    double  cyl_radius,               cyl_length;


    int     sphere_radius_unit_steps, sphere_translation_unit_steps;
    double  sphere_radius,            sphere_translation;

};

struct BuddedCylinderParams setupHealthyBuddedCylinder(void);

#endif
