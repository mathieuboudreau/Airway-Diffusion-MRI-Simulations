include_directories ("${CMAKE_SOURCE_DIR}/src/params")
include_directories ("${CMAKE_SOURCE_DIR}/src/morphology")

add_library(simulation bloch_torrey.c)

target_include_directories(simulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(simulation
                      params
                      morphology
                      m)
