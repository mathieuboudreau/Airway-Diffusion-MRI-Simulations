#include <stdlib.h>
#include "generatevolume.h"

void allocvolume(struct Volume * v)
{

    /* allocate each row's place holder */
    v->array = malloc(sizeof(int*) * v->xdim);

    /* allocate each row's columns */
    for(int x = 0; x < v->xdim; x++){
        v->array[x] = malloc(sizeof(int*) * v->ydim);
        for(int y = 0; y < v->ydim; y++){
            v->array[x][y] = malloc(sizeof(int*) * v->zdim);
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