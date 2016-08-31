#include <check.h>
#include "test_generatevolume.h"

Suite * generatevolume_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("generatevolume.h test");

    /* Core test case */
    tc_core = tcase_create("Core");

    /* alloc() */
    tcase_add_test(tc_core, test_that_allocvolume_returns_array_with_dimensions_that_match_func_arg);
    
    /* generatevolume() */
    tcase_add_test(tc_core, test_that_generatevolume_with_cylinder_arg_returns_properly_defined_cylinder_mask);
    
    suite_add_tcase(s, tc_core);

    return s;
}