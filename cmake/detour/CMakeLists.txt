project(detour)
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

SET( prefix ${DEPENDENCY_SOURCES}/${PROJECT_NAME} )
SET( hprefix ${DEPENDENCY_INCLUDE_ROOT}/${PROJECT_NAME} )

SET(sources
DetourAlloc.cpp
DetourCommon.cpp
DetourNavMesh.cpp
DetourNavMeshBuilder.cpp
DetourNavMeshQuery.cpp
DetourNode.cpp
DetourObstacleAvoidance.cpp)

SET(headers
DetourAlloc.h
DetourAssert.h
DetourCommon.h
DetourNavMesh.h
DetourNavMeshBuilder.h
DetourNavMeshQuery.h
DetourNode.h
DetourObstacleAvoidance.h)

foreach(src IN ITEMS ${sources} )
  SET( SRCS ${SRCS} ${prefix}/${src} )
endforeach(src)
foreach(src IN ITEMS ${headers} )
  SET( HEADERS ${HEADERS} ${hprefix}/${src} )
endforeach(src)


include_directories( ${GLOBAL_INCLUDE_DIRS} )
add_library(${PROJECT_NAME} STATIC ${SRCS} ${HEADERS} )
#Set it to the project name.
set(DETOUR_LIBRARIES ${PROJECT_NAME} CACHE INTERNAL "Detour Libraries")