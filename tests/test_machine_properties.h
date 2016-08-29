#include <check.h>
#include "../src/util/machine_properties.h"

/* Begin: float  f_calc_epsilon() & double d_calc_epsilon() */
START_TEST(test_that_float_epsilon_is_larger_than_double_epsilon)
{
    ck_assert(f_calc_epsilon() > d_calc_epsilon());
}
END_TEST

/* End: float  f_calc_epsilon() & double d_calc_epsilon() */
