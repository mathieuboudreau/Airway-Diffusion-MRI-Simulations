#include <check.h>
#include "test_buddedcylinder.h"

Suite * buddedcylinder_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("buddedcylinder.h test");

    /* Core test case */
    tc_core = tcase_create("Core");

    /* cylinder() */
    tcase_add_test(tc_core, test_point_inside_buddedcylinder_cyl);
    tcase_add_test(tc_core, test_point_inside_buddecylinder_top_sphere);
    tcase_add_test(tc_core, test_point_inside_buddecylinder_bottom_sphere);
    tcase_add_test(tc_core, test_point_on_surface_of_buddecylinder_top_sphere);
    tcase_add_test(tc_core, test_point_outside_buddedcylinder);

    suite_add_tcase(s, tc_core);

    return s;
}