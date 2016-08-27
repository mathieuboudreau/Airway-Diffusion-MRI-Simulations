include_directories ("${CMAKE_SOURCE_DIR}/src/morphology")
include_directories ("${CMAKE_SOURCE_DIR}/src/util")

# add the executable
add_executable (test_cylinder test_cylinder.c)
set_target_properties(test_cylinder PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)
target_link_libraries(test_cylinder check gsl morphology util)
