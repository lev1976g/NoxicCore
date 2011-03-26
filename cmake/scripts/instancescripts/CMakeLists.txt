if(NOT BUILD_INSTANCESCRIPTS )
  message("\tInstanceScripts was skipped because BUILD_INSTANCESCRIPTS is unchecked.")
  return()
endif(NOT BUILD_INSTANCESCRIPTS)

PROJECT(InstanceScripts)

SET( prefix ${SCRIPTS_PATH}/InstanceScripts )
#dependancies that must be built before this project.
SET( dependancies arcemu-world )
#libraries/archives that must be linked to this project.
SET( linkages arcemu-world )

set(sources
Instance_Arcatraz.cpp 
Instance_AuchenaiCrypts.cpp 
Instance_AzjolNerub.cpp 
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
Setup.cpp )

foreach(src IN ITEMS ${sources} )
  SET( SRCS ${SRCS} ${prefix}/${src} )
endforeach(src)

include_directories( ${include_dirs} )
add_definitions(-Wno-deprecated)
add_library( ${PROJECT_NAME} SHARED ${SRCS} )
add_dependencies( ${PROJECT_NAME} ${dependancies} )
#target_link_libraries( ${PROJECT_NAME} ${linkages} )
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
#Cleap up our variables(Not required.)
unset( prefix)
unset( SRCS )
unset( sources )
unset( dependancies )
unset( linkages)