project(PATH64)
cmake_minimum_required(VERSION 2.8)

set(DEBUG_BUILD_FLAGS "-W -Wall -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Wmissing-format-attribute" CACHE INTERNAL "" FORCE)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_BUILD_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_BUILD_FLAGS}" CACHE STRING "" FORCE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(ConfigureChecks)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(AFTER SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DTARG_X8664 -DKEY -DFE_GNU_4_2_0 -DIs_True_On -DHAVE_CONFIG_H)

add_subdirectory(ekopath-gcc-4.2.0)
add_subdirectory(libspin)
add_subdirectory(pathscale)
