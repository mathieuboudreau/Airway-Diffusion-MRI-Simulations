include_directories ("${CMAKE_SOURCE_DIR}/src/morphology")
include_directories ("${CMAKE_SOURCE_DIR}/src/params")
include_directories ("${CMAKE_SOURCE_DIR}/src/util")
include_directories ("${CMAKE_SOURCE_DIR}/src/simulation")

# add the executable
add_executable (unit_tests unit_tests.c)
set_target_properties(unit_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)

add_library(test_shapes test_shapes.c)
add_library(test_buddedcylinder test_buddedcylinder.c)
add_library(test_generatevolume test_generatevolume.c)
add_library(test_machine_properties test_machine_properties.c)
add_library(test_bloch_torrey test_bloch_torrey.c)

target_link_libraries(unit_tests
                      check
                      gsl
                      morphology
                      util
                      params
                      simulation
                      m
                      test_shapes
                      test_buddedcylinder
                      test_generatevolume
                      test_machine_properties
                      test_bloch_torrey)
