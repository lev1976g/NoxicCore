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

PROJECT(vmap_assembler CXX)

SET(prefix ${ROOT_PATH}/src/tools/vmap_tools/vmap_assembler)
SET(sources
	vmap_assembler.cpp
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${DEPENDENCY_INCLUDE_ROOT}/vmaplib ${DEPENDENCY_INCLUDE_ROOT}/g3dlite)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})

IF(APPLE)
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} vmaplib g3dlite ${ZLIB_LIBRARIES} "-framework Carbon")
ELSE(APPLE)
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} vmaplib g3dlite ${ZLIB_LIBRARIES})
ENDIF(APPLE)

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_TOOLS_PATH})