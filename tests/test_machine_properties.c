#include <check.h>
#include "test_machine_properties.h"

Suite * machine_properties_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("machine_properties.h test");
    
    /* Core test case */
    tc_core = tcase_create("Core");
    
    /* cylinder() */
    tcase_add_test(tc_core, test_that_float_epsilon_is_larger_than_double_epsilon);

    suite_add_tcase(s, tc_core);

    return s;
}