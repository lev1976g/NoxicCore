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

PROJECT(QuestScripts)

SET(prefix ${SCRIPTS_PATH}/${PROJECT_NAME})
SET(sources
	Setup.cpp 
	../Common/Base.cpp 
	Quest_ArathiHighlands.cpp 
	Quest_Azshara.cpp 
	Quest_Azuremyst_Isle.cpp 
	Quest_BladeEdge_Mountains.cpp 
	Quest_BlastedLands.cpp 
	Quest_BloodmystIsle.cpp 
	Quest_BoreanTundra.cpp 
	Quest_BurningSteppes.cpp 
	Quest_Darkshore.cpp 
	Quest_DeathKnight.cpp 
	Quest_Desolace.cpp 
	Quest_Dragonblight.cpp 
	Quest_Druid.cpp 
	Quest_Duskwood.cpp 
	Quest_Dustwallow_Marsh.cpp 
	Quest_Eastern_Plaguelands.cpp 
	Quest_EversongWoods.cpp
	Quest_Feralas.cpp 
	Quest_FirstAid.cpp 
	Quest_Ghostlands.cpp 
	Quest_Hellfire_Peninsula.cpp 
	Quest_HillsbradFoothills.cpp 
	Quest_Icecrown.cpp
	Quest_Isle_of_QuelDanas.cpp
	Quest_Howling_Fjord.cpp 
	Quest_LochModan.cpp 
	Quest_Mage.cpp 
	Quest_Mulgore.cpp 
	Quest_Nagrand.cpp 
	Quest_Netherstorm.cpp 
	Quest_Paladin.cpp 
	QuestGossip.cpp 
	QuestHooks.cpp 
	Quest_RedridgeMountains.cpp 
	Quest_ShadowMoon.cpp 
	Quest_SholazarBasin.cpp 
	Quest_Silithus.cpp 
	Quest_SilvermoonCity.cpp 
	Quest_Silverpine_Forest.cpp 
	Quest_Stormwind.cpp 
	Quest_StranglethornVale.cpp 
	Quest_Tanaris.cpp 
	Quest_Teldrassil.cpp 
	Quest_Terrokar_Forest.cpp 
	Quest_The_Barrens.cpp 
	Quest_TheStormPeaks.cpp 
	Quest_ThousandNeedles.cpp 
	Quest_TirisfalGlades.cpp 
	Quest_Undercity.cpp 
	Quest_UnGoro.cpp 
	Unsorted.cpp 
	Quest_Warrior.cpp 
	Quest_Westfall.cpp 
	Quest_Wetlands.cpp
	Quest_Winterspring.cpp 
	Quest_Zangarmarsh.cpp
)

SET(headers
	Setup.h
	../Common/Base.h
	../Common/EasyFunctions.h
	../Common/Instance_Base.h
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)
FOREACH(src IN ITEMS ${headers})
	SET(HEADERS ${HEADERS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${INCLUDE_dirs} ${prefix})
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