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
SET( prefix ${ROOT_PATH}/src/tools/map_tools)
SET( sources
	System.cpp
)

foreach(src IN ITEMS ${sources} )
  SET( SRCS ${SRCS} ${prefix}/${src} )
endforeach(src)

include_directories( ${DEPENDENCY_INCLUDE_ROOT}/libmpq_new ${DEPENDENCY_INCLUDE_ROOT}/dbcfile ${DEPENDENCY_INCLUDE_ROOT}/loadlib )

ADD_EXECUTABLE( ${PROJECT_NAME} ${SRCS} )

target_link_libraries( ${PROJECT_NAME} dbcfile loadlib ${ZLIB_LIBRARIES} )

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_TOOLS_PATH} )