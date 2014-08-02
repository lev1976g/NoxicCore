IF(NOT BUILD_TOOLS)
  message(WARNING, "BUILD_TOOLS option is set to false. Tool files will not be built.")
  return()
ENDIF(NOT BUILD_TOOLS)

SET(ARCEMU_TOOLS_PATH "tools" CACHE PATH "The directory where you want the tools installed." )

add_subdirectory(bzip2)
add_subdirectory(libmpq_new)
add_subdirectory(dbcfile)
add_subdirectory(loadlib)

add_subdirectory(map_tools)
add_subdirectory(creature_data)
add_subdirectory(vmap_tools)