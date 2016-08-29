#include <check.h>
#include "test_shapes.h"

Suite * shapes_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("shapes.h test");
    
    /* Core test case */
    tc_core = tcase_create("Core");
    
    /* cylinder() */
    tcase_add_test(tc_core, test_point_inside_cylinder);
    tcase_add_test(tc_core, test_point_outside_cylinder);
    tcase_add_test(tc_core, test_point_on_the_surface_of_cylinder_flat);
    tcase_add_test(tc_core, test_point_on_the_surface_of_cylinder_curve);
    tcase_add_test(tc_core, test_point_on_the_surface_of_cylinder_corner_of_both_flat_curve);

    tcase_add_test(tc_core, test_point_inside_cylinder_with_translated_origin);
    tcase_add_test(tc_core, test_point_on_the_surface_of_cylinder_corner_of_both_flat_curve_with_translated_origin);


    /* sphere() */
    tcase_add_test(tc_core, test_point_inside_sphere);
    tcase_add_test(tc_core, test_point_outside_sphere);
    tcase_add_test(tc_core, test_point_on_the_surface_of_sphere);
    tcase_add_test(tc_core, test_point_inside_sphere_with_translated_origin);
    tcase_add_test(tc_core, test_point_on_the_surface_of_sphere_with_translated_origin);

    suite_add_tcase(s, tc_core);

    return s;
}