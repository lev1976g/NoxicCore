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

IF(NOT BUILD_LOGON)
	MESSAGE(WARNING, "BUILD_LOGON option is set to FALSE. Logon server will not be built.")
	return()
ENDIF(NOT BUILD_LOGON)

PROJECT(logon CXX)
SET(prefix ${ROOT_PATH}/src/arcemu-logonserver)
SET(sources
	AccountCache.cpp
	AuthSocket.cpp
	AutoPatcher.cpp
	LogonCommServer.cpp
	LogonConsole.cpp
	LogonOpcodes.cpp
	LogonStdAfx.cpp
	Main.cpp
)

SET(headers
	AccountCache.h
	AuthSocket.h
	AuthStructs.h
	AutoPatcher.h
	LogonCommServer.h
	LogonConsole.h
	LogonOpcodes.h
	LogonStdAfx.h
	Main.h
	PeriodicFunctionCall_Thread.h
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)
FOREACH(src IN ITEMS ${headers})
	SET(HEADERS ${HEADERS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${GLOBAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${EXTRA_LIBS_PATH} ${DEPENDENCY_LIBS})
IF(IS_64BIT)
	LINK_DIRECTORIES(${DEPENDENCY_DLLS64})
ENDIF()

INCLUDE(PCHSupport)

SET(WIN_RESOURCE_FILE ${ROOT_PATH}/src/arcemu-logonserver/resources.rc)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS} ${HEADERS} ${WIN_RESOURCE_FILE})

IF(CMAKE_GENERATOR MATCHES Visual*)
	ADD_NATIVE_PRECOMPILED_HEADER(${PROJECT_NAME} LogonStdAfx.h ${prefix}/LogonStdAfx.cpp)
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} shared ${ZLIB_LIBRARIES} ${PCRE_LIBRARIES})

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)