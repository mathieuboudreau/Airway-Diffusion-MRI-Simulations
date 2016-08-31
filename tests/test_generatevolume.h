#include <check.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include "../src/morphology/generatevolume.h"
#include "../src/morphology/buddedcylinder.h"

/* Begin: int allocvolume(const int * dims[3]) */

START_TEST(test_that_allocvolume_returns_array_with_dimensions_that_match_func_arg)
{
    struct Volume * volume;

    volume->xdim = 10;
    volume->ydim = 10;
    volume->zdim = 10;

    allocvolume(volume);

    ck_assert(volume->array[9][9][9]==0);

    freevolume(volume);
}
END_TEST

/* End: int allocvolume() */


/* Begin: int allocvolume(const int * dims[3]) */

START_TEST(test_that_generatevolume_with_cylinder_arg_returns_properly_defined_cylinder_mask)
{
    struct Volume volume;

    volume.xdim = 92;
    volume.ydim = 92;
    volume.zdim = 72;
    double sqrt2 = M_SQRT2;

    allocvolume(&volume);

    generatevolume(&volume, &buddedcylinder);
    
    ck_assert(volume.array[volume.xdim/2-1][volume.ydim/2-1][volume.zdim/2-1]==1);
    ck_assert(volume.array[(volume.xdim/2-1) + 24 + 16][(volume.ydim/2-1)][(volume.zdim/4 * 3) - 1]==1);
    ck_assert(volume.array[(volume.xdim/2-1) + (int)ceil((24 + 16)/sqrt2)][(volume.ydim/2-1) + (int)ceil((24 + 16)/sqrt2)][(volume.zdim/4) - 1]==1);

    freevolume(&volume);
}
END_TEST

/* End: int allocvolume() */
