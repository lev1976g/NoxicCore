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

IF(NOT BUILD_LUAENGINE)
	MESSAGE("\tLuaEngine was skipped because BUILD_LUAENGINE is unchecked.")
	return()
ENDIF(NOT BUILD_LUAENGINE)

PROJECT(LuaEngine)

SET(prefix ${SCRIPTS_PATH}/${PROJECT_NAME} )
SET(SRCS
	${prefix}/LUAEngine.cpp 
)

SET(HEADERS
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

INCLUDE_DIRECTORIES( ${INCLUDE_dirs} ${prefix} )
IF(IS_LINUX)
	ADD_DEFINITIONS(-Wno-deprecated)
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS} ${HEADERS})
ADD_DEPENDENCIES( ${PROJECT_NAME} ${dependancies} lualib)
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${linkages} lualib)
INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_SCRIPTLIB_PATH} LIBRARY DESTINATION ${ARCEMU_SCRIPTLIB_PATH})

unSET(prefix)
unSET(SRCS)
unSET(HEADERS)