#include <gsl/gsl_math.h>
#include "shapes.h"

int buddedcylinder(const double v[3],
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
    double cyl_origin[3];

    cyl_origin[0] = 0.0;
    cyl_origin[1] = 0.0;
    cyl_origin[2] = 0.0;

    /* Setup top row of spheres */
 
    double top_sph_origin_px[3];
    double top_sph_origin_nx[3];
    double top_sph_origin_py[3];
    double top_sph_origin_ny[3];

    top_sph_origin_px[0] = sphere_translation;
    top_sph_origin_px[1] = 0.0;
    top_sph_origin_px[2] = top_height;

    top_sph_origin_nx[0] = -sphere_translation;
    top_sph_origin_nx[1] = 0.0;
    top_sph_origin_nx[2] = top_height;

    top_sph_origin_py[0] = 0.0;
    top_sph_origin_py[1] = sphere_translation;
    top_sph_origin_py[2] = top_height;

    top_sph_origin_ny[0] = 0.0;
    top_sph_origin_ny[1] = -sphere_translation;
    top_sph_origin_ny[2] = top_height;

    /* Setup bottom row of spheres */

    double bottom_sph_origin_pxpy[3];
    double bottom_sph_origin_nxpy[3];
    double bottom_sph_origin_pxny[3];
    double bottom_sph_origin_nxny[3];

    bottom_sph_origin_pxpy[0] = sphere_translation/sqrt2;
    bottom_sph_origin_pxpy[1] = sphere_translation/sqrt2;
    bottom_sph_origin_pxpy[2] = bottom_height;

    bottom_sph_origin_nxpy[0] = -sphere_translation/sqrt2;
    bottom_sph_origin_nxpy[1] = sphere_translation/sqrt2;
    bottom_sph_origin_nxpy[2] = bottom_height;

    bottom_sph_origin_pxny[0] = sphere_translation/sqrt2;
    bottom_sph_origin_pxny[1] = -sphere_translation/sqrt2;
    bottom_sph_origin_pxny[2] = bottom_height;

    bottom_sph_origin_nxny[0] = -sphere_translation/sqrt2;
    bottom_sph_origin_nxny[1] = -sphere_translation/sqrt2;
    bottom_sph_origin_nxny[2] = bottom_height;


    /***Assert if point is located inside or on the surface of the budded cylinder***/

    /* Center cylinder */
    if(cylinder(v, cyl_origin, cyl_radius, cyl_length)){
        return 1;
    }

    /* Top row of spheres */
    else if(sphere(v, top_sph_origin_px, sphere_radius)){
        return 1;
    }
    else if(sphere(v, top_sph_origin_nx, sphere_radius)){
        return 1;
    }
    else if(sphere(v, top_sph_origin_py, sphere_radius)){
        return 1;
    }
    else if(sphere(v, top_sph_origin_ny, sphere_radius)){
        return 1;
    }

    /* Bottom row of spheres */
    else if(sphere(v, bottom_sph_origin_pxpy, sphere_radius)){
        return 1;
    }
    else if(sphere(v, bottom_sph_origin_nxpy, sphere_radius)){
        return 1;
    }
    else if(sphere(v, bottom_sph_origin_pxny, sphere_radius)){
        return 1;
    }
    else if(sphere(v, bottom_sph_origin_nxny, sphere_radius)){
        return 1;
    }


    else{
        return 0;
    }

}