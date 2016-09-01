add_subdirectory (morphology)
add_subdirectory (params)
add_subdirectory (util)

# add the executable
add_executable (main main.c)
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)

target_link_libraries (main morphology
                            params
                            util)

target_include_directories (main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add the executable
add_executable (volume_2_hdf5 volume_2_hdf5.c)
set_target_properties(volume_2_hdf5 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)

target_link_libraries (volume_2_hdf5 gsl
                                     hdf5
                                     morphology
                                     params
                                     util)

target_include_directories (volume_2_hdf5 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
