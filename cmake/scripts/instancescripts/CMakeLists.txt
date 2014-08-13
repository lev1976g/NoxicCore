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

PROJECT(InstanceScripts)

SET(prefix ${SCRIPTS_PATH}/InstanceScripts)
SET(sources
	Instance_Arcatraz.cpp 
	Instance_AuchenaiCrypts.cpp 
	Instance_AzjolNerub.cpp 
	Instance_BlackfathomDeeps.cpp 
	Instance_BlackMorass.cpp 
	Instance_BlackrockDepths.cpp 
	Instance_BlackrockSpire.cpp 
	Instance_BloodFurnace.cpp 
	Instance_Botanica.cpp 
	Instance_CullingOfStratholme.cpp 
	Instance_Deadmines.cpp 
	Instance_DireMaul.cpp 
	Instance_DrakTharonKeep.cpp 
	Instance_Gundrak.cpp 
	Instance_HallsOfLightning.cpp 
	Instance_HellfireRamparts.cpp 
	Instance_MagistersTerrace.cpp 
	Instance_ManaTombs.cpp 
	Instance_Mauradon.cpp 
	Instance_Nexus.cpp 
	Instance_OldHillsbradFoothills.cpp 
	Instance_HallsOfStone.cpp 
	Instance_RagefireChasm.cpp 
	Instance_RazorfenDowns.cpp 
	Instance_ScarletMonastery.cpp 
	Instance_Scholomance.cpp 
	Instance_SethekkHalls.cpp 
	Instance_ShadowLabyrinth.cpp 
	Instance_ShadowfangKeep.cpp 
	Instance_TheMechanar.cpp 
	Instance_TheShatteredHalls.cpp 
	Instance_TheSlavePens.cpp 
	Instance_TheSteamvault.cpp 
	Instance_TheUnderbog.cpp 
	Instance_Uldaman.cpp 
	Instance_UtgardeKeep.cpp 
	Instance_WailingCaverns.cpp 
	Instance_TheStockade.cpp 
	Instance_ZulFarrak.cpp 
	Raid_BlackTemple.cpp 
	Raid_BlackwingLair.cpp 
	Raid_CoT_BattleOfMountHyjal.cpp 
	Raid_GruulsLair.cpp
	Raid_IceCrownCitadel.cpp
	Raid_Karazhan.cpp 
	Raid_MoltenCore.cpp 
	Raid_Naxxramas.cpp 
	Raid_OnyxiasLair.cpp 
	Raid_SerpentshrineCavern.cpp 
	Raid_TheEye.cpp 
	Raid_TheObsidianSanctum.cpp 
	Raid_Ulduar.cpp 
	Raid_ZulGurub.cpp 
	Raid_Magtheridons_Lair.cpp 
	SUNWELL_PLAT/Raid_SunwellPlateau.cpp 
	Raid_WorldBosses.cpp 
	Raid_ZulAman.cpp 
	../Common/Instance_Base.cpp 
	../Common/Base.cpp 
	Setup.cpp
)

SET(headers
	Raid_IceCrownCitadel.h
	Raid_Naxxramas.h
	Raid_Ulduar.h
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