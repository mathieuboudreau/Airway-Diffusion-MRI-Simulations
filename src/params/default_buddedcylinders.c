#include <gsl/gsl_const_num.h>
#include "default_buddedcylinders.h"

struct BuddedCylinderParams setupHealthyBuddedCylinder(void)
{
    struct BuddedCylinderParams healthyBudCyl;

    healthyBudCyl.dx = healthyBudCyl.dy = healthyBudCyl.dz = 8.75 * GSL_CONST_NUM_MICRO;

    healthyBudCyl.xdim = healthyBudCyl.ydim     = 92;
    healthyBudCyl.zdim                          = 72;

    healthyBudCyl.cyl_radius_unit_steps         = 22;
    healthyBudCyl.cyl_length_unit_steps         = 72;

    healthyBudCyl.cyl_radius                    = healthyBudCyl.cyl_radius_unit_steps         * healthyBudCyl.dx;
    healthyBudCyl.cyl_length                    = healthyBudCyl.cyl_length_unit_steps         * healthyBudCyl.dz;

    healthyBudCyl.sphere_radius_unit_steps      = 16;
    healthyBudCyl.sphere_translation_unit_steps = 24;

    healthyBudCyl.sphere_radius                 = healthyBudCyl.sphere_radius_unit_steps      * healthyBudCyl.dx;
    healthyBudCyl.sphere_translation            = healthyBudCyl.sphere_translation_unit_steps * healthyBudCyl.dx;

    return healthyBudCyl;

}

struct BuddedCylinderParams setupDiseasedBuddedCylinder(void)
{
    struct BuddedCylinderParams diseaseBudCyl;

    diseaseBudCyl.dx = diseaseBudCyl.dy = diseaseBudCyl.dz = 8.75 * GSL_CONST_NUM_MICRO ;

    diseaseBudCyl.xdim = diseaseBudCyl.ydim     = 92 ;
    diseaseBudCyl.zdim                          = 72 ;

    diseaseBudCyl.cyl_radius_unit_steps         = 32 ;
    diseaseBudCyl.cyl_length_unit_steps         = 72 ;

    diseaseBudCyl.cyl_radius                    = diseaseBudCyl.cyl_radius_unit_steps         * diseaseBudCyl.dx;
    diseaseBudCyl.cyl_length                    = diseaseBudCyl.cyl_length_unit_steps         * diseaseBudCyl.dz;

    diseaseBudCyl.sphere_radius_unit_steps      = 16  ;
    diseaseBudCyl.sphere_translation_unit_steps = 24  ;

    diseaseBudCyl.sphere_radius                 = diseaseBudCyl.sphere_radius_unit_steps      * diseaseBudCyl.dx;
    diseaseBudCyl.sphere_translation            = diseaseBudCyl.sphere_translation_unit_steps * diseaseBudCyl.dx;

    return diseaseBudCyl;

}