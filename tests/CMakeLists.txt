include_directories ("${CMAKE_SOURCE_DIR}/src/morphology")
include_directories ("${CMAKE_SOURCE_DIR}/src/util")

# add the executable
add_executable (test_shapes test_shapes.c)
set_target_properties(test_shapes PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)
target_link_libraries(test_shapes check gsl morphology util)
