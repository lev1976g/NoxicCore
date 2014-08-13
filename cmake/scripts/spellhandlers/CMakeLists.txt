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

PROJECT(SpellHandlers)

SET(prefix ${ROOT_PATH}/src/scripts/src/${PROJECT_NAME})
SET(sources
	DeathKnightSpells.cpp
	DruidSpells.cpp
	HunterSpells.cpp
	ItemSpells_1.cpp
	MageSpells.cpp
	PaladinSpells.cpp
	PetAISpells.cpp
	PriestSpells.cpp
	QIspells.cpp
	RogueSpells.cpp
	Setup.cpp
	ShamanSpells.cpp
	WarlockSpells.cpp
	WarriorSpells.cpp
	MiscSpells.cpp
)

FOREACH(src IN ITEMS ${sources})
  SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${GLOBAL_INCLUDE_DIRS})
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})
ADD_DEPENDENCIES(${PROJECT_NAME} shared world)
ADD_DEFINITIONS(-DSCRIPTLIB -DSKIP_ALLOCATOR_SHARING)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} world)

INCLUDE(PCHSupport)
IF( CMAKE_GENERATOR MATCHES Visual*)
	ADD_NATIVE_PRECOMPILED_HEADER(${PROJECT_NAME} Setup.h ${prefix}/Setup.cpp)
ENDIF()

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_SCRIPTLIB_PATH} LIBRARY DESTINATION ${ARCEMU_SCRIPTLIB_PATH} )

unSET(prefix)
unSET(SRCS)
unSET(sources)