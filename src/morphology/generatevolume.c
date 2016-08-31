#include <stdlib.h>
#include <gsl/gsl_vector.h>
#include "generatevolume.h"

#define R_D 22       // Radius of inner cylinder (units steps)
#define R 24         // Position of the sphere center of with respect to the center of the cylinder (or lengths) (units steps)
#define r 2.1e-4     // Meter conversion of R (m)
#define r_a 1.4e-4                         // Radius of sphere? (m)
#define L_A 18                             // 1/4 of length of the cylinder (units steps)
#define R_A 16

void generatevolume(struct Volume * v, int (*shape)() )
{

    double r_d=r*R_D/R;      // Size of R_D (m)
    double dx=r_d/R_D;       // Step size in x direction (m)
    double dy=dx;            // Step size in y direction (m)
    double dz=dx;            // Step size in z direction (m)
             


    double cyl_radius         = (double)R_D * ( (double)r/(double)R );
    double cyl_length         = (double)4   * (double)L_A * dz ;
    double sphere_radius      = (double)r_a;
    double sphere_translation = (double)R   * dx;

    gsl_vector * p = gsl_vector_alloc (3);
    gsl_vector * origin = gsl_vector_alloc (3);

    for(int x = 0; x < v->xdim; x++){
        for(int y = 0; y < v->ydim; y++){
            for(int z = 0; z < v->zdim; z++){
                gsl_vector_set (p, 0, (x - (double)(92+1)/(double)2) * dx);
                gsl_vector_set (p, 1, (y - (double)(92+1)/(double)2) * dy);
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