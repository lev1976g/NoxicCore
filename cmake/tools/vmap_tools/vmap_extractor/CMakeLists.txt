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

PROJECT(vmap_extractor CXX)
SET( prefix ${ROOT_PATH}/src/tools/vmap_tools/vmap_extractor)
SET( sources
adtfile.cpp
dbcfile.cpp
model.cpp
vmapexport.cpp
wdtfile.cpp
wmo.cpp
)

SET( headers
adtfile.h
dbcfile.h
model.h
modelheaders.h
vec3d.h
vmapexport.h
wdtfile.h
wmo.h
)

foreach(src IN ITEMS ${sources} )
  SET( SRCS ${SRCS} ${prefix}/${src} )
endforeach(src)
foreach(src IN ITEMS ${headers} )
  SET( HEADERS ${HEADERS} ${prefix}/${src} )
endforeach(src)

IF( NOT WIN32 AND NOT IS_64BIT )
	add_definitions( -D_FILE_OFFSET_BITS=64 )
ENDIF( NOT WIN32 AND NOT IS_64BIT )

include_directories( ${DEPENDENCY_INCLUDE_ROOT}/libmpq_new ${DEPENDENCY_INCLUDE_ROOT}/loadlib ${DEPENDENCY_INCLUDE_ROOT}/zlib ${DEPENDENCY_INCLUDE_ROOT}/bzip2 )

ADD_EXECUTABLE( ${PROJECT_NAME} ${SRCS} ${HEADERS} )

target_link_libraries( ${PROJECT_NAME} libmpq_new loadlib ${ZLIB_LIBRARIES} bzip2 )

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_TOOLS_PATH} )

