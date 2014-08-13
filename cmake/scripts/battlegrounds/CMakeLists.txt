PROJECT(Battlegrounds)
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

SET( prefix ${ROOT_PATH}/src/scripts/src/${PROJECT_NAME} )

#sources -contains source files for our library.
SET( sources
    AlteracValley.cpp
    ArathiBasin.cpp
    CircleOfBlood.cpp
    DalaranSewers.cpp
    EyeOfTheStorm.cpp
    IsleOfConquest.cpp
    RingOfTrials.cpp
    RingOfValor.cpp
    RuinsOfLordaeron.cpp
    Setup.cpp
    StrandOfTheAncient.cpp
    WarsongGulch.cpp
 )

SET( headers
	AlteracValley.h
    ArathiBasin.h
    CircleOfBlood.h
    DalaranSewers.h
    EyeOfTheStorm.h
    IsleOfConquest.h
    RingOfTrials.h
    RingOfValor.h
    RuinsOfLordaeron.h
    Setup.h
    StrandOfTheAncient.h
    WarsongGulch.h
 )
 
foreach(src IN ITEMS ${sources} )
  SET( SRCS ${SRCS} ${prefix}/${src} )
endforeach(src)
foreach(src IN ITEMS ${headers} )
  SET( HEADERS ${HEADERS} ${prefix}/${src} )
endforeach(src)

include_directories( ${GLOBAL_INCLUDE_DIRS} )
add_library( ${PROJECT_NAME} SHARED ${SRCS} ${HEADERS})
add_dependencies( ${PROJECT_NAME} shared world )
add_definitions( -DSCRIPTLIB -DSKIP_ALLOCATOR_SHARING )
target_link_libraries( ${PROJECT_NAME} world )

include(PCHSupport)
IF( CMAKE_GENERATOR MATCHES Visual*)
	ADD_NATIVE_PRECOMPILED_HEADER(${PROJECT_NAME} Setup.h ${prefix}/Setup.cpp)
ENDIF()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_SCRIPTLIB_PATH} LIBRARY DESTINATION ${ARCEMU_SCRIPTLIB_PATH} )
#Cleap up our variables(Not required.)
unset( prefix )
unset( SRCS )
unset( HEADERS )
unset( headers )
unset( sources )
