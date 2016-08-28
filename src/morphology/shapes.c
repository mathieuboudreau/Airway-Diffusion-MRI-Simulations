#include <gsl/gsl_math.h>
#include <gsl/gsl_sys.h>
#include <gsl/gsl_vector.h>
#include "../util/calcepsilon.h"

int cylinder(const gsl_vector * v, const double cyl_radius, const double cyl_length)
{
    /* Evaluates if a 3D point is inside a cylinder who's bottom is fixed at the center origin.
    
    To Do: Add check that cyl_radius and cyl_length are positive
    */
    
    double d_eps = d_calc_epsilon();
    
    double x = gsl_vector_get(v, 0);
    double y = gsl_vector_get(v, 1);
    double z = gsl_vector_get(v, 2);
    
    
    /* In-plane distance squared of point away from origin */
    double d_dist_sq   = gsl_pow_2(x)+gsl_pow_2(y);
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

int sphere(const gsl_vector * v, const double sph_radius)
{
    /* Evaluates if a 3D point is inside a sphere who's center is fixed at the origin.
    
    To Do: Add check that sph_radius positive.
    */
    
    double d_eps = d_calc_epsilon();
    
    double x = gsl_vector_get(v, 0);
    double y = gsl_vector_get(v, 1);
    double z = gsl_vector_get(v, 2);
    
    
    /* In-plane distance squared of point away from origin */
    double d_dist_sq   = gsl_pow_2(x)+gsl_pow_2(y)+gsl_pow_2(z);
    double d_radius_sq = gsl_pow_2(sph_radius);
    

    if ( (d_dist_sq<d_radius_sq) || !gsl_fcmp(d_dist_sq, d_radius_sq, d_eps) ) {
        return 1; 
    } else{
        return 0;
    }
}