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

PROJECT(vmaplib)

SET(prefix ${DEPENDENCY_SOURCES}/${PROJECT_NAME})
SET(sources
	BIH.cpp
	MapTree.cpp
	ModelInstance.cpp
	TileAssembler.cpp
	VMapFactory.cpp
	VMapManager2.cpp
	WorldModel.cpp
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${GLOBAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DEPENDENCY_LIBS} ${EXTRA_LIBS_PATH})
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} g3dlite)

unSET(sources)
unSET(SRCS)
unSET(prefix)