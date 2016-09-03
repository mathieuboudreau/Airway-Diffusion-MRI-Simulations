#include <stdio.h>
#include "hdf5.h"
#include <string.h>

#include "../src/morphology/generatevolume.h"
#include "../src/morphology/buddedcylinder.h"
#include "../src/params/default_buddedcylinders.h"

int main(int argc, char *argv[]) 
{
    char * filename = argv[1];

    hid_t       file_id, dataset_id, dataspace_id;  /* identifiers */
    herr_t      status;

    struct BuddedCylinderParams bcParams;
    if(strcmp(argv[2], "healthy") == 0){
        bcParams = setupHealthyBuddedCylinder();
    }  
    else if(strcmp(argv[2], "disease") == 0){
        bcParams = setupDiseasedBuddedCylinder();
    }  
    else{
        printf("Second argument must be \"healthy\" or \"disease\".\n");
        return 1;
    }

    /* Create a new file using default properties. */
    file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    struct Volume volume;
    volume.xdim = bcParams.xdim;
    volume.ydim = bcParams.ydim;
    volume.zdim = bcParams.zdim;

    allocvolume(&volume);
    generatevolume(&volume, &buddedcylinder, &bcParams);
    
    /***** Write volume to HDF5 file*****/    
    hsize_t dims[3] = {volume.xdim,volume.ydim,volume.zdim};
    dataspace_id = H5Screate_simple(3, dims, NULL);
    
    /* Create the dataset. */
    dataset_id = H5Dcreate2(file_id, "volume", H5T_STD_I32BE, dataspace_id, H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);

    /* buffer for write */
    int wdata[volume.xdim][volume.ydim][volume.zdim];
    for(int i = 0; i < volume.xdim; i++){
        for(int j = 0; j < volume.ydim; j++){
            for(int k = 0; k < volume.zdim; k++){
                wdata[i][j][k] = volume.array[i][j][k];
            }
        }
    }

    /* Write the dataset. */
    status = H5Dwrite(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      wdata);
    
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    /* Terminate access to the data space. */ 
    status = H5Sclose(dataspace_id);
    /* Close the file. */
    status = H5Fclose(file_id);

    /* Deallocate Volume */
    freevolume(&volume);

    return 0;
}