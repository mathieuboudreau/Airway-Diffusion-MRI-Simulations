add_subdirectory (morphology)
add_subdirectory (params)
add_subdirectory (util)
add_subdirectory (simulation)

find_package(HDF5 COMPONENTS C)

# add the executable
add_executable (main main.c)
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)

target_link_libraries (main morphology
                            params
                            util)

target_include_directories (main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(HDF5_FOUND)
# add the executable
add_executable (volume_2_hdf5 volume_2_hdf5.c)
set_target_properties(volume_2_hdf5 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)

target_link_libraries (volume_2_hdf5 gsl
                                     ${HDF5_C_LIBRARIES}
                                     morphology
                                     params
                                     util)

target_include_directories (volume_2_hdf5 PUBLIC
                             ${CMAKE_CURRENT_SOURCE_DIR}
                             ${HDF5_INCLUDE_DIRS})
endif()

# add the executable
add_executable (diffusion_sim diffusion_sim.c)
set_target_properties(diffusion_sim PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)

target_link_libraries (diffusion_sim gsl
                                     morphology
                                     util
                                     params
                                     simulation
                                     m)

target_include_directories (diffusion_sim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# comparison helper: runs angle n=1 with the original 2011 parameters
add_executable (compare_2011 compare_2011.c)
set_target_properties(compare_2011 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)

target_link_libraries (compare_2011 gsl
                                    morphology
                                    util
                                    params
                                    simulation
                                    m)

target_include_directories (compare_2011 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
