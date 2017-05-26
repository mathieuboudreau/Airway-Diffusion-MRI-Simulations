#ifndef _TEST_GENERATEVOLUME_H
#define _TEST_GENERATEVOLUME_H

#include <check.h>
#include <gsl/gsl_math.h>
#include "../src/morphology/generatevolume.h"
#include "../src/morphology/buddedcylinder.h"
#include "../src/params/default_buddedcylinders.h"

/* Begin: int allocvolume(const int * dims[3]) */

START_TEST(test_that_allocvolume_returns_array_with_dimensions_that_match_func_arg)
{
    struct Volume volume;

    volume.xdim = 10;
    volume.ydim = 10;
    volume.zdim = 10;

    allocvolume(&volume);

    ck_assert(volume.array[9][9][9]==0);

    freevolume(&volume);
}
END_TEST

/* End: int allocvolume() */


/* Begin: int allocvolume(const int * dims[3]) */

START_TEST(test_that_generatevolume_with_cylinder_arg_returns_properly_defined_cylinder_mask)
{
    struct BuddedCylinderParams bcParams = setupHealthyBuddedCylinder();

    struct Volume volume;
    volume.xdim = bcParams.xdim;
    volume.ydim = bcParams.ydim;
    volume.zdim = bcParams.zdim;

    double sqrt2 = M_SQRT2;

    allocvolume(&volume);

    generatevolume(&volume, &buddedcylinder, &bcParams);
    
    ck_assert(volume.array[bcParams.xdim/2-1][bcParams.ydim/2-1][bcParams.zdim/2-1]==1);
    ck_assert(volume.array[(bcParams.xdim/2-1) + bcParams.sphere_translation_unit_steps + bcParams.sphere_radius_unit_steps][bcParams.ydim/2-1][(bcParams.zdim/4 * 3) - 1]==1);
    ck_assert(volume.array[(bcParams.xdim/2-1) + (int)ceil((double)(bcParams.sphere_translation_unit_steps + bcParams.sphere_radius_unit_steps)/sqrt2)][(bcParams.ydim/2-1) + (int)ceil((double)(bcParams.sphere_translation_unit_steps + bcParams.sphere_radius_unit_steps)/sqrt2)][(bcParams.zdim/4) - 1]==1);

    freevolume(&volume);
}
END_TEST

/* End: int allocvolume() */

#endif