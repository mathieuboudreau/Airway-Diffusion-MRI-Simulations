#include <stdlib.h>
#include <gsl/gsl_vector.h>
#include "generatevolume.h"
#include "../params/default_buddedcylinders.h"

void generatevolume(struct Volume * v, int (*shape)(), struct BuddedCylinderParams * params)
{

    double dx = params->dx;            // Step size in x direction (m)
    double dy = params->dy;            // Step size in y direction (m)
    double dz = params->dz;            // Step size in z direction (m)


    double cyl_radius                              = params->cyl_radius;
    double cyl_length                              = params->cyl_length;
    double sphere_radius                           = params->sphere_radius;
    double sphere_translation                      = params->sphere_translation;

    double cyl_recenter_translation_x_dunit_steps  = (double)(params->xdim+1)/(double)2;
    double cyl_recenter_translation_y_dunit_steps  = (double)(params->ydim+1)/(double)2;

    gsl_vector * p = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    for(int x = 0; x < v->xdim; x++){
        for(int y = 0; y < v->ydim; y++){
            for(int z = 0; z < v->zdim; z++){
                gsl_vector_set (p, 0, (x - cyl_recenter_translation_x_dunit_steps) * dx);
                gsl_vector_set (p, 1, (y - cyl_recenter_translation_y_dunit_steps) * dy);
                gsl_vector_set (p, 2, z * dz);

                v->array[x][y][z] = shape(p, cyl_radius, cyl_length, sphere_radius, sphere_translation);
            }
        }
    }

}

void allocvolume(struct Volume * v)
{

    /* allocate each row's place holder and initialize to 0 */
    v->array = calloc(v->xdim, sizeof(int*));

    /* allocate each row's columns and initialize to 0 */
    for(int x = 0; x < v->xdim; x++){
        v->array[x] = calloc(v->ydim, sizeof(int*));
        for(int y = 0; y < v->ydim; y++){
            v->array[x][y] = calloc(v->zdim,sizeof(int*));
        }
    }

}

void freevolume(struct Volume * v)
{

    for(int x = 0; x < v->xdim; x++){
        for(int y = 0; y < v->ydim; y++){
            free(v->array[x][y]);
        }
        free(v->array[x]);
    }
    free(v->array);

}