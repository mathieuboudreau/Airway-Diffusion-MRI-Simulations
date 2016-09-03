#ifndef _TEST_SHAPES_H
#define _TEST_SHAPES_H

#include <check.h>
#include "../src/morphology/shapes.h"

/* Begin: int cylinder(const double v[3], const double cyl_radius, const double cyl_length) */
START_TEST(test_point_inside_cylinder)
{
    double v[3];
    double origin[3];

    v[0] = 1.0;
    v[1] = 1.0;
    v[2] = 1.0;

    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 0.0;

    double cyl_radius = 2.0;
    double cyl_length = 2.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

START_TEST(test_point_outside_cylinder)
{
    double v[3];
    double origin[3];

    v[0] = 1.0;
    v[1] = 1.0;
    v[2] = 1.0;

    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 0.0;

    double cyl_radius = 0.5;
    double cyl_length = 0.5;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)0);
}
END_TEST

START_TEST(test_point_on_the_surface_of_cylinder_flat)
{
    double v[3];
    double origin[3];

    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 1.0;

    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 0.0;

    double cyl_radius = 1.0;
    double cyl_length = 1.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

START_TEST(test_point_on_the_surface_of_cylinder_curve)
{
    double v[3];
    double origin[3];

    v[0] = 0.0;
    v[1] = 1.0;
    v[2] = 0.5;

    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 0.0;

    double cyl_radius = 1.0;
    double cyl_length = 1.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST


START_TEST(test_point_on_the_surface_of_cylinder_corner_of_both_flat_curve)
{
    double v[3];
    double origin[3];

    v[0] = 0.0;
    v[1] = 1.0;
    v[2] = 1.0;

    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 0.0;

    double cyl_radius = 1.0;
    double cyl_length = 1.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

START_TEST(test_point_inside_cylinder_with_translated_origin)
{
    double v[3];
    double origin[3];

    v[0] = 6.0;
    v[1] = 6.0;
    v[2] = 1.0;

    origin[0] = 5.0;
    origin[1] = 5.0;
    origin[2] = 0.0;

    double cyl_radius = 2.0;
    double cyl_length = 2.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

START_TEST(test_point_on_the_surface_of_cylinder_corner_of_both_flat_curve_with_translated_origin)
{
    double v[3];
    double origin[3];

    v[0] = 3.0;
    v[1] = 4.0;
    v[2] = 1.0;

    origin[0] = 3.0;
    origin[1] = 3.0;
    origin[2] = 0.0;

    double cyl_radius = 1.0;
    double cyl_length = 1.0;

    ck_assert_int_eq(cylinder(v, origin, cyl_radius, cyl_length),(int)1);
}
END_TEST

/* End: int cylinder(const double v[3], const double cyl_radius, const double cyl_length) */

/* Begin: int sphere  (const double v[3], const double sph_radius) */

START_TEST(test_point_inside_sphere)
{
    double v[3];
    double origin[3];

    v[0] = 1.0;
    v[1] = 1.0;
    v[2] = 1.0;

    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 0.0;

    double sph_radius = 2.0;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)1);
}
END_TEST

START_TEST(test_point_outside_sphere)
{
    double v[3];
    double origin[3];

    v[0] = 1.0;
    v[1] = 1.0;
    v[2] = 1.0;

    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 0.0;

    double sph_radius = 0.5;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)0);
}
END_TEST

START_TEST(test_point_on_the_surface_of_sphere)
{
    double v[3];
    double origin[3];

    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 1.0;

    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 0.0;

    double sph_radius = 1.0;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)1);
}
END_TEST

START_TEST(test_point_inside_sphere_with_translated_origin)
{
    double v[3];
    double origin[3];

    v[0] = 6.0;
    v[1] = 1.0;
    v[2] = 1.0;

    origin[0] = 5.0;
    origin[1] = 0.0;
    origin[2] = 0.0;

    double sph_radius = 2.0;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)1);
}
END_TEST

START_TEST(test_point_on_the_surface_of_sphere_with_translated_origin)
{
    double v[3];
    double origin[3];

    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 1.0;

    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 2.0;

    double sph_radius = 1.0;

    ck_assert_int_eq(sphere(v, origin, sph_radius),(int)1);
}
END_TEST

/* End: int sphere  (double v[3], const double sph_radius) */

#endif