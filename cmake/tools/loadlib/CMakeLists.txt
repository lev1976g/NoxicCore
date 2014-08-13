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

PROJECT(loadlib)

SET(prefix ${DEPENDENCY_SOURCES}/${PROJECT_NAME})
SET(sources
	adt.cpp
	loadlib.cpp
	wdt.cpp
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${DEPENDENCY_INCLUDE_ROOT}/${PROJECT_NAME} ${DEPENDENCY_INCLUDE_ROOT}/libmpq_new)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} libmpq_new)

unSET(sources)
unSET(SRCS)
unSET(prefix)