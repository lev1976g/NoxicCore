PROJECT(libmpq_new)
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

set(prefix ${DEPENDENCY_SOURCES}/${PROJECT_NAME} )

set(sources
common.c
explode.c
extract.c
huffman.c
mpq.c
mpq_libmpq.cpp
wave.c
)

foreach(src IN ITEMS ${sources} )
  SET( SRCS ${SRCS} ${prefix}/${src} )
endforeach(src)

IF( NOT WIN32 AND NOT IS_64BIT )
	add_definitions( -D_FILE_OFFSET_BITS=64 )
ENDIF( NOT WIN32 AND NOT IS_64BIT )

include_directories( ${DEPENDENCY_INCLUDE_ROOT}/${PROJECT_NAME} ${DEPENDENCY_INCLUDE_ROOT}/zlib ${DEPENDENCY_INCLUDE_ROOT}/bzip2 ${DEPENDENCY_INCLUDE_ROOT}/loadlib )
add_library(${PROJECT_NAME} STATIC ${SRCS} )
target_link_libraries( ${PROJECT_NAME} ${ZLIB_LIBRARIES} bzip2 )

unset(sources)
unset(SRCS)
unset(prefix)

