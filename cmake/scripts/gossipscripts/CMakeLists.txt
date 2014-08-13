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

PROJECT(GossipScripts)

SET(prefix ${SCRIPTS_PATH}/GossipScripts)
SET(sources
	Gossip_Dalaran.cpp
	Gossip_Innkeepers.cpp
	Gossip_Guard.cpp
	Gossip_Trainer.cpp
	Gossip_Mulgore.cpp
	Gossip_Shattrath.cpp
	Gossip_Tanaris.cpp
	Gossip_Stormwind.cpp
	Gossip_Moonglade.cpp
	Gossip_DarkmoonFaire.cpp
	Gossip_Theramore.cpp
	Gossip_Teldrassil.cpp
	Gossip_BoreanTundra.cpp
	Gossip_WyrmrestTemple.cpp
	Gossip_XpEliminator.cpp
	Setup.cpp
)

SET(headers
	Setup.h
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)
FOREACH(src IN ITEMS ${headers})
	SET(HEADERS ${HEADERS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${INCLUDE_dirs})
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS} ${HEADERS})
ADD_DEPENDENCIES(${PROJECT_NAME} ${dependancies})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${linkages})

INCLUDE(PCHSupport)
IF(CMAKE_GENERATOR MATCHES Visual*)
	ADD_NATIVE_PRECOMPILED_HEADER(${PROJECT_NAME} Setup.h ${prefix}/Setup.cpp)
ENDIF()

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${ARCEMU_SCRIPTLIB_PATH} LIBRARY DESTINATION ${ARCEMU_SCRIPTLIB_PATH})

unSET(prefix)
unSET(SRCS)
unSET(sources)