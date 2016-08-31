#include <check.h>
#include <gsl/gsl_vector.h>
#include "../src/morphology/generatevolume.h"

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