#child CMakeLists use this variable to access the source file directories. 
#e.g ${SCRIPTS_PATH}/LuaEngine to access the source files for LuaEngine.

if( NOT BUILD_ARCEMUSCRIPTS )
  message( WARNING, "BUILD_ARCEMUSCRIPTS option is set to false. Script libraries will not be built." )
  return()
endif(NOT BUILD_ARCEMUSCRIPTS)
message("Configuring scripts :")
SET( SCRIPTS_PATH ${ROOT_PATH}/src/scripts/src )
#common include directories that script libraries might include.
SET( include_dirs ${ROOT_PATH}/src/arcemu-world ${ROOT_PATH}/src/arcemu-shared)

#All script libraries have SCRIPTLIB defined.
add_definitions(-DSCRIPTLIB)
SET( BUILD_EVENTSCRIPTS TRUE CACHE BOOL "Build arcEventScripts.")
SET( BUILD_INSTANCESCRIPTS TRUE CACHE BOOL "Build arcInstanceScripts.")
SET( BUILD_EXTRASCRIPTS TRUE CACHE BOOL "Build arcExtraScripts.")
SET( BUILD_GOSSIPSCRIPTS TRUE CACHE BOOL "Build arcGossipScripts.")
SET( BUILD_QUESTSCRIPTS TRUE CACHE BOOL "Build arcQuestScripts.")
SET( BUILD_MISCSCRIPTS TRUE CACHE BOOL "Build arcMiscScripts." )
SET( BUILD_SPELLHANDLERS TRUE CACHE BOOL "Build arcSpellHandlers.")
SET( BUILD_WORLDPVPSCRIPTS TRUE CACHE BOOL "Build WorldPVPScripts.")
SET( BUILD_LUAENGINE FALSE CACHE BOOL "Build LuaEngine.")
SET( BUILD_LUABRIDGE FALSE CACHE BOOL "Build luabridge.")

add_subdirectory( luabridge ) 
add_subdirectory( luaengine )
add_subdirectory( eventscripts)
add_subdirectory( instancescripts )
add_subdirectory( extrascripts )
add_subdirectory( gossipscripts )
add_subdirectory( questscripts )
add_subdirectory( miscscripts )
add_subdirectory( spellhandlers )
add_subdirectory( worldpvpscripts )

unset(SCRIPTS_PATH)
unset(include_dirs)