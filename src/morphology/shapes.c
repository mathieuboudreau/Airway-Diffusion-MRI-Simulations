#include <gsl/gsl_math.h>
#include <gsl/gsl_sys.h>
#include "../util/machine_properties.h"

int cylinder(const double v[3], const double origin[3], const double cyl_radius, const double cyl_length)
{
    /* Evaluates if a 3D point is inside a cylinder who's bottom is fixed at the center origin.
    
    To Do: Add check that cyl_radius and cyl_length are positive
    */
    
    double d_eps = d_calc_epsilon();
    
    double x = v[0];
    double y = v[1];
    double z = v[2];
    
    double ox = origin[0];
    double oy = origin[1];
    
    /* In-plane distance squared of point away from origin */
    double d_dist_sq   = gsl_pow_2(x-ox)+gsl_pow_2(y-oy);
    double d_radius_sq = gsl_pow_2(cyl_radius);
    

    if ( (d_dist_sq<d_radius_sq) || !gsl_fcmp(d_dist_sq, d_radius_sq, d_eps) ) {
        if( (z < cyl_length) || !gsl_fcmp(z, cyl_length, d_eps) ){
            return 1;
        } else {
            return 0;
        }    
    } else{
        return 0;
    }
}

int sphere(const double v[3], const double origin[3], const double sph_radius)
{
    /* Evaluates if a 3D point is inside a sphere positioned at an arbitrary origin.
    
    To Do: Add check that sph_radius positive.
    */
    
    double d_eps = d_calc_epsilon();
    
    double x = v[0];
    double y = v[1];
    double z = v[2];
    
    double ox = origin[0];
    double oy = origin[1];
    double oz = origin[2];

    /* In-plane distance squared of point away from origin */
    double d_dist_sq   = gsl_pow_2(x-ox)+gsl_pow_2(y-oy)+gsl_pow_2(z-oz);
    double d_radius_sq = gsl_pow_2(sph_radius);
    

    if ( (d_dist_sq<d_radius_sq) || !gsl_fcmp(d_dist_sq, d_radius_sq, d_eps) ) {
        return 1; 
    } else{
        return 0;
    }
}