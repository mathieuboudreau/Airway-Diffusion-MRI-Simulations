#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include "shapes.h"

int buddedcylinder(const gsl_vector * v,
                   const double cyl_radius,
                   const double cyl_length,
                   const double sphere_radius,
                   const double sphere_translation)
{

    /* Setup constants*/
    double top_height    = 0.75 * cyl_length;
    double bottom_height = 0.25 * cyl_length;

    double sqrt2 = M_SQRT2;

    /* Setup cylinder */
    gsl_vector * cyl_origin = gsl_vector_alloc (3);

    gsl_vector_set (cyl_origin, 0, 0.0);
    gsl_vector_set (cyl_origin, 1, 0.0);
    gsl_vector_set (cyl_origin, 2, 0.0);

    /* Setup top row of spheres */
 
    gsl_vector * top_sph_origin_px = gsl_vector_alloc (3);
    gsl_vector * top_sph_origin_nx = gsl_vector_alloc (3);
    gsl_vector * top_sph_origin_py = gsl_vector_alloc (3);
    gsl_vector * top_sph_origin_ny = gsl_vector_alloc (3);

    gsl_vector_set (top_sph_origin_px, 0, sphere_translation);
    gsl_vector_set (top_sph_origin_px, 1, 0.0);
    gsl_vector_set (top_sph_origin_px, 2, top_height);

    gsl_vector_set (top_sph_origin_nx, 0, -sphere_translation);
    gsl_vector_set (top_sph_origin_nx, 1, 0.0);
    gsl_vector_set (top_sph_origin_nx, 2, top_height);

    gsl_vector_set (top_sph_origin_py, 0, 0.0);
    gsl_vector_set (top_sph_origin_py, 1, sphere_translation);
    gsl_vector_set (top_sph_origin_py, 2, top_height);

    gsl_vector_set (top_sph_origin_ny, 0, 0.0);
    gsl_vector_set (top_sph_origin_ny, 1, -sphere_translation);
    gsl_vector_set (top_sph_origin_ny, 2, top_height);

    /* Setup bottom row of spheres */

    gsl_vector * bottom_sph_origin_pxpy = gsl_vector_alloc (3);
    gsl_vector * bottom_sph_origin_nxpy = gsl_vector_alloc (3);
    gsl_vector * bottom_sph_origin_pxny = gsl_vector_alloc (3);
    gsl_vector * bottom_sph_origin_nxny = gsl_vector_alloc (3);

    gsl_vector_set (bottom_sph_origin_pxpy, 0, sphere_translation/sqrt2);
    gsl_vector_set (bottom_sph_origin_pxpy, 1, sphere_translation/sqrt2);
    gsl_vector_set (bottom_sph_origin_pxpy, 2, bottom_height);

    gsl_vector_set (bottom_sph_origin_nxpy, 0, -sphere_translation/sqrt2);
    gsl_vector_set (bottom_sph_origin_nxpy, 1, sphere_translation/sqrt2);
    gsl_vector_set (bottom_sph_origin_nxpy, 2, bottom_height);

    gsl_vector_set (bottom_sph_origin_pxny, 0, sphere_translation/sqrt2);
    gsl_vector_set (bottom_sph_origin_pxny, 1, -sphere_translation/sqrt2);
    gsl_vector_set (bottom_sph_origin_pxny, 2, bottom_height);

    gsl_vector_set (bottom_sph_origin_nxny, 0, -sphere_translation/sqrt2);
    gsl_vector_set (bottom_sph_origin_nxny, 1, -sphere_translation/sqrt2);
    gsl_vector_set (bottom_sph_origin_nxny, 2, bottom_height);


    /***Assert if point is located inside or on the surface of the budded cylinder***/

    /* Center cylinder */
    if(cylinder(v, cyl_origin, cyl_radius, cyl_length)){
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 1;
    }

    /* Top row of spheres */
    else if(sphere(v, top_sph_origin_px, sphere_radius)){
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 1;
    }
    else if(sphere(v, top_sph_origin_nx, sphere_radius)){
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 1;
    }
    else if(sphere(v, top_sph_origin_py, sphere_radius)){
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 1;
    }
    else if(sphere(v, top_sph_origin_ny, sphere_radius)){
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 1;
    }

    /* Bottom row of spheres */
    else if(sphere(v, bottom_sph_origin_pxpy, sphere_radius)){
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 1;
    }
    else if(sphere(v, bottom_sph_origin_nxpy, sphere_radius)){
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 1;
    }
    else if(sphere(v, bottom_sph_origin_pxny, sphere_radius)){
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 1;
    }
    else if(sphere(v, bottom_sph_origin_nxny, sphere_radius)){
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 1;
    }


    else{
        gsl_vector_free(cyl_origin);
        gsl_vector_free(top_sph_origin_px);
        gsl_vector_free(top_sph_origin_nx);
        gsl_vector_free(top_sph_origin_py);
        gsl_vector_free(top_sph_origin_ny);
        gsl_vector_free(bottom_sph_origin_pxpy);
        gsl_vector_free(bottom_sph_origin_nxpy);
        gsl_vector_free(bottom_sph_origin_pxny);
        gsl_vector_free(bottom_sph_origin_nxny);
        return 0;
    }

}