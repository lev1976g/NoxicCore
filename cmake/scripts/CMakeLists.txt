###############################################################################
# NoxicCore MMORPG Server
# Copyright (c) 2011-2014 Crimoxic Team
# Copyright (c) 2008-2014 ArcEmu Team <http://www.arcemu.org/>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

#child CMakeLists use this variable to access the source file directories. 
#e.g ${SCRIPTS_PATH}/LuaEngine to access the source files for LuaEngine.

if( NOT BUILD_SCRIPTS )
  message( WARNING, "BUILD_SCRIPTS option is set to false. Script libraries will not be built." )
  return()
endif(NOT BUILD_SCRIPTS)

message("Configuring scripts...")

SET( SCRIPTS_PATH ${ROOT_PATH}/src/scripts/src )
#common include directories that script libraries might include.
SET( include_dirs ${GLOBAL_INCLUDE_DIRS} )
SET( dependancies shared world )
SET( linkages world)

#All script libraries have SCRIPTLIB defined.
add_definitions(-DSCRIPTLIB -DSKIP_ALLOCATOR_SHARING)
SET(BUILD_LUAENGINE TRUE CACHE BOOL "Build ArcEmu Lua Engine (ALE).")

add_subdirectory(eventscripts)
add_subdirectory(instancescripts)
add_subdirectory(extrascripts)
add_subdirectory(gossipscripts)
add_subdirectory(questscripts)
add_subdirectory(miscscripts)
add_subdirectory(serverstatusplugin)
add_subdirectory(lualib) #Used by the LuaEngine module.
add_subdirectory(luaengine)
add_subdirectory(spellhandlers)
add_subdirectory(battlegrounds)

unset(SCRIPTS_PATH)
unset(include_dirs)