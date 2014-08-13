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

PROJECT(map_tools CXX)

SET(prefix ${ROOT_PATH}/src/tools/map_tools)
SET(sources
	System.cpp
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${DEPENDENCY_INCLUDE_ROOT}/libmpq_new ${DEPENDENCY_INCLUDE_ROOT}/dbcfile ${DEPENDENCY_INCLUDE_ROOT}/loadlib)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} dbcfile loadlib ${ZLIB_LIBRARIES})

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_TOOLS_PATH})