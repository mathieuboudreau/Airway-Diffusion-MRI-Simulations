add_library(morphology shapes.c
                       buddedcylinder.c
                       generatevolume.c)

target_include_directories (morphology PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(morphology gsl)
