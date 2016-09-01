#ifndef _TEST_BUDDEDCYLINDER_H
#define _TEST_BUDDEDCYLINDER_H

#include <check.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include "../src/morphology/buddedcylinder.h"

/* Begin: int buddedcylinder(const gsl_vector * v,
                             const double cyl_radius,
                             const double cyl_length,
                             const double sphere_radius,
                             const double sphere_translation); */

START_TEST(test_point_inside_buddedcylinder_cyl)
{
    double cyl_radius         = 2.0;
    double cyl_length         = 8.0;
    double sphere_radius      = 2.0;
    double sphere_translation = 2.0;

    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 0.0);
    gsl_vector_set (v, 1, 0.0);
    gsl_vector_set (v, 2, 4.0);

    ck_assert_int_eq(buddedcylinder(v, cyl_radius, cyl_length, sphere_radius, sphere_translation),(int)1);
}
END_TEST

START_TEST(test_point_inside_buddecylinder_top_sphere)
{
    double cyl_radius         = 2.0;
    double cyl_length         = 12.0;
    double sphere_radius      = 2.0;
    double sphere_translation = 2.0;

    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 3.0);
    gsl_vector_set (v, 1, 0.0);
    gsl_vector_set (v, 2, 9.0);

    ck_assert_int_eq(buddedcylinder(v, cyl_radius, cyl_length, sphere_radius, sphere_translation),(int)1);
}
END_TEST

START_TEST(test_point_inside_buddecylinder_bottom_sphere)
{
    double sqrt2 = M_SQRT2;

    double cyl_radius         = 2.0;
    double cyl_length         = 12.0;
    double sphere_radius      = 2.0;
    double sphere_translation = 2.0;

    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 3.0/sqrt2);
    gsl_vector_set (v, 1, 3.0/sqrt2);
    gsl_vector_set (v, 2, 3.0);

    ck_assert_int_eq(buddedcylinder(v, cyl_radius, cyl_length, sphere_radius, sphere_translation),(int)1);
}
END_TEST

START_TEST(test_point_on_surface_of_buddecylinder_top_sphere)
{
    double cyl_radius         = 2.0;
    double cyl_length         = 12.0;
    double sphere_radius      = 2.0;
    double sphere_translation = 2.0;

    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 0.0);
    gsl_vector_set (v, 1, -4.0);
    gsl_vector_set (v, 2, 9.0);

    ck_assert_int_eq(buddedcylinder(v, cyl_radius, cyl_length, sphere_radius, sphere_translation),(int)1);
}
END_TEST

START_TEST(test_point_outside_buddedcylinder)
{
    double cyl_radius         = 2.0;
    double cyl_length         = 8.0;
    double sphere_radius      = 2.0;
    double sphere_translation = 2.0;

    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 5.0);
    gsl_vector_set (v, 1, 0.0);
    gsl_vector_set (v, 2, 4.0);

    ck_assert_int_eq(buddedcylinder(v, cyl_radius, cyl_length, sphere_radius, sphere_translation),(int)0);
}
END_TEST
/* End: int buddedcylinder() */

#endif
