#include <check.h>
#include <gsl/gsl_vector.h>
#include "../src/morphology/shapes.h"

/* Begin: int cylinder(const gsl_vector * v, const double cyl_radius, const double cyl_length) */
START_TEST(test_point_inside_cylinder)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 1.0);
    gsl_vector_set (v, 1, 1.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 0.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 0.0);

    double cyl_radius = 2.0;
    double cyl_length = 2.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

START_TEST(test_point_outside_cylinder)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 1.0);
    gsl_vector_set (v, 1, 1.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 0.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 0.0);

    double cyl_radius = 0.5;
    double cyl_length = 0.5;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)0);
}
END_TEST

START_TEST(test_point_on_the_surface_of_cylinder_flat)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 0.0);
    gsl_vector_set (v, 1, 0.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 0.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 0.0);

    double cyl_radius = 1.0;
    double cyl_length = 1.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

START_TEST(test_point_on_the_surface_of_cylinder_curve)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 0.0);
    gsl_vector_set (v, 1, 1.0);
    gsl_vector_set (v, 2, 0.5);

    gsl_vector_set (origin, 0, 0.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 0.0);

    double cyl_radius = 1.0;
    double cyl_length = 1.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST


START_TEST(test_point_on_the_surface_of_cylinder_corner_of_both_flat_curve)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 0.0);
    gsl_vector_set (v, 1, 1.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 0.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 0.0);

    double cyl_radius = 1.0;
    double cyl_length = 1.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

START_TEST(test_point_inside_cylinder_with_translated_origin)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 6.0);
    gsl_vector_set (v, 1, 6.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 5.0);
    gsl_vector_set (origin, 1, 5.0);
    gsl_vector_set (origin, 2, 0.0);

    double cyl_radius = 2.0;
    double cyl_length = 2.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

START_TEST(test_point_on_the_surface_of_cylinder_corner_of_both_flat_curve_with_translated_origin)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 3.0);
    gsl_vector_set (v, 1, 4.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 3.0);
    gsl_vector_set (origin, 1, 3.0);
    gsl_vector_set (origin, 2, 0.0);

    double cyl_radius = 1.0;
    double cyl_length = 1.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

/* End: int cylinder(const gsl_vector * v, const double cyl_radius, const double cyl_length) */

/* Begin: int sphere  (const gsl_vector * v, const double sph_radius) */

START_TEST(test_point_inside_sphere)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 1.0);
    gsl_vector_set (v, 1, 1.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 0.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 0.0);

    double sph_radius = 2.0;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)1);
}
END_TEST

START_TEST(test_point_outside_sphere)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 1.0);
    gsl_vector_set (v, 1, 1.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 0.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 0.0);

    double sph_radius = 0.5;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)0);
}
END_TEST

START_TEST(test_point_on_the_surface_of_sphere)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 0.0);
    gsl_vector_set (v, 1, 0.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 0.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 0.0);

    double sph_radius = 1.0;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)1);
}
END_TEST

START_TEST(test_point_inside_sphere_with_translated_origin)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 6.0);
    gsl_vector_set (v, 1, 1.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 5.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 0.0);

    double sph_radius = 2.0;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)1);
}
END_TEST

START_TEST(test_point_on_the_surface_of_sphere_with_translated_origin)
{
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    gsl_vector_set (v, 0, 0.0);
    gsl_vector_set (v, 1, 0.0);
    gsl_vector_set (v, 2, 1.0);

    gsl_vector_set (origin, 0, 0.0);
    gsl_vector_set (origin, 1, 0.0);
    gsl_vector_set (origin, 2, 2.0);

    double sph_radius = 1.0;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)1);
}
END_TEST

/* End: int sphere  (const gsl_vector * v, const double sph_radius) */
