include_directories(BEFORE SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

# GENMODES
add_custom_target(genmodes-commands)
add_custom_command(TARGET genmodes-commands
		   COMMAND "/bin/sh"
                     ARGS "${CMAKE_CURRENT_SOURCE_DIR}/mkconfig.sh" "bconfig.h")

set(GENMODES_SRCS genmodes.c
                  errors.c)

add_executable(genmodes ${GENMODES_SRCS})
add_dependencies(genmodes genmodes-commands)
target_link_libraries(genmodes iberty)

# MISC
add_custom_target(misc-gcc-commands)
add_custom_command(TARGET misc-gcc-commands
		   COMMAND genmodes
		     ARGS -h >inst-modes.h
		   MAIN_DEPENDENCY genmodes)

# GENCHECKS
add_custom_target(gencheck-commands)
add_custom_command(TARGET gencheck-commands
                   COMMAND "/bin/sh"
                     ARGS "${CMAKE_CURRENT_SOURCE_DIR}/cp/mkgencheck.sh" >gencheck.h)

set(GENCHECK_SRCS gencheck.c)

add_executable(gencheck ${GENCHECK_SRCS})
add_dependencies(gencheck genmodes-commands gencheck-commands)
target_link_libraries(gencheck iberty)
