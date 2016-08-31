#include <stdlib.h>
#include "generatevolume.h"

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