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

PROJECT(EventScripts)

SET(prefix ${SCRIPTS_PATH}/${PROJECT_NAME})
SET(sources
	Setup.cpp
	Event_Halloween.cpp
	Event_WinterVeil.cpp
	../Common/Base.cpp
)

SET(headers
	Setup.h
	../Common/Base.h
	../Common/EasyFunctions.h
	../Common/Instance_Base.h
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)
FOREACH(src IN ITEMS ${headers})
	SET(HEADERS ${HEADERS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${INCLUDE_dirs} ${prefix})
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS} ${HEADERS})
ADD_DEPENDENCIES(${PROJECT_NAME} ${dependancies})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${linkages})

INCLUDE(PCHSupport)
IF(CMAKE_GENERATOR MATCHES Visual*)
	ADD_NATIVE_PRECOMPILED_HEADER(${PROJECT_NAME} Setup.h ${prefix}/Setup.cpp)
ENDIF()

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_SCRIPTLIB_PATH} LIBRARY DESTINATION ${ARCEMU_SCRIPTLIB_PATH})

unSET(prefix)
unSET(SRCS)
unSET(sources)