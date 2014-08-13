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

IF(NOT BUILD_SCRIPTS)
	MESSAGE(WARNING, "BUILD_SCRIPTS option is set to FALSE. Script libraries will not be built.")
	return()
ENDIF(NOT BUILD_SCRIPTS)

MESSAGE("Configuring scripts...")

SET(SCRIPTS_PATH ${ROOT_PATH}/src/scripts/src)
#common INCLUDE directories that script libraries might INCLUDE.
SET(INCLUDE_dirs ${GLOBAL_INCLUDE_DIRS} )
SET(dependancies shared world )
SET(linkages world)

#All script libraries have SCRIPTLIB defined.
ADD_DEFINITIONS(-DSCRIPTLIB -DSKIP_ALLOCATOR_SHARING)
SET(BUILD_LUAENGINE TRUE CACHE BOOL "Build ArcEmu Lua Engine (ALE).")

ADD_SUBDIRECTORY(eventscripts)
ADD_SUBDIRECTORY(instancescripts)
ADD_SUBDIRECTORY(extrascripts)
ADD_SUBDIRECTORY(gossipscripts)
ADD_SUBDIRECTORY(questscripts)
ADD_SUBDIRECTORY(miscscripts)
ADD_SUBDIRECTORY(serverstatusplugin)
ADD_SUBDIRECTORY(lualib) #Used by the LuaEngine module.
ADD_SUBDIRECTORY(luaengine)
ADD_SUBDIRECTORY(spellhandlers)
ADD_SUBDIRECTORY(battlegrounds)

unSET(SCRIPTS_PATH)
unSET(INCLUDE_dirs)