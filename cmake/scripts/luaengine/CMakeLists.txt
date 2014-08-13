if(NOT BUILD_LUAENGINE)
  message("\tLuaEngine was skipped because BUILD_LUAENGINE is unchecked.")
  return()
endif(NOT BUILD_LUAENGINE)

if(BUILD_LUABRIDGE)
	message(ERROR, "\tBUILD_LUABRIDGE is enabled but luabridge cannot run together with LuaEngine!")
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

	return()
endif(BUILD_LUABRIDGE)

project(LuaEngine)

#prefix variable - points to the folder which contains the source files.
SET( prefix ${SCRIPTS_PATH}/${PROJECT_NAME} )

#SRCS - contains the source files that compose this project.
SET( SRCS
${prefix}/LUAEngine.cpp 
)

SET( HEADERS
${prefix}/AuraFunctions.h
${prefix}/FunctionTables.h
${prefix}/GameobjectFunctions.h
${prefix}/GlobalFunctions.h
${prefix}/ItemFunctions.h
${prefix}/LUAEngine.h
${prefix}/LUAFunctions.h
${prefix}/LuaSqlApi.h
${prefix}/PacketFunctions.h
${prefix}/SpellFunctions.h
${prefix}/TaxiFunctions.h
${prefix}/UnitFunctions.h
)

include_directories( ${include_dirs} ${prefix} )
IF( IS_LINUX )
	add_definitions(-Wno-deprecated)
ENDIF()


add_library(${PROJECT_NAME} SHARED ${SRCS} ${HEADERS})
add_dependencies( ${PROJECT_NAME} ${dependancies} lualib)
target_link_libraries( ${PROJECT_NAME} ${linkages} lualib)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_SCRIPTLIB_PATH} LIBRARY DESTINATION ${ARCEMU_SCRIPTLIB_PATH})

#Cleap up our variables(Not required.)
unset( prefix)
unset( SRCS )
unset( HEADERS )
