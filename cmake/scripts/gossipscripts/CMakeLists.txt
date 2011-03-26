if(NOT BUILD_GOSSIPSCRIPTS )
  message("\tGossipScripts was skipped because BUILD_GOSSIPSCRIPTS is unchecked.")
  return()
endif(NOT BUILD_GOSSIPSCRIPTS)

PROJECT(GossipScripts)

SET( prefix ${SCRIPTS_PATH}/GossipScripts )
#dependancies that must be built before this project.
SET( dependancies arcemu-world )
#libraries/archives that must be linked to this project.
SET( linkages arcemu-world )
#SRCS - contains the source files that compose this project.

set( sources
Gossip_Battlemaster.cpp
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