#ifndef _SHAPES_H
#define _SHAPES_H

int cylinder(const gsl_vector * v, const double cyl_radius, const double cyl_length);
int sphere  (const gsl_vector * v, const gsl_vector * origin, const double sph_radius);

#endif
