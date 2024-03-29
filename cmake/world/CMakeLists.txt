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

IF(NOT BUILD_WORLD)
	MESSAGE(WARNING, "BUILD_WORLD option is SET to false. World server will not be built." )
	return()
ENDIF(NOT BUILD_WORLD)

PROJECT(world)
SET( prefix ${ROOT_PATH}/src/arcemu-world )
SET(sources
	Commands/cmd_account.cpp
	Commands/cmd_achievement.cpp
	Commands/cmd_admin.cpp
	Commands/cmd_arena.cpp
	Commands/cmd_ban.cpp
	Commands/cmd_battleground.cpp
	Commands/cmd_character.cpp
	Commands/cmd_cheat.cpp
	Commands/cmd_debug.cpp
	Commands/cmd_event.cpp
	Commands/cmd_gamemaster.cpp
	Commands/cmd_go.cpp
	Commands/cmd_gobject.cpp
	Commands/cmd_group.cpp
	Commands/cmd_guild.cpp
	Commands/cmd_honor.cpp
	Commands/cmd_instance.cpp
	Commands/cmd_item.cpp
	Commands/cmd_learn.cpp
	Commands/cmd_lfg.cpp
	Commands/cmd_list.cpp
	Commands/cmd_lookup.cpp
	Commands/cmd_message.cpp
	Commands/cmd_misc.cpp
	Commands/cmd_mmaps.cpp
	Commands/cmd_modify.cpp
	Commands/cmd_npc.cpp
	Commands/cmd_pet.cpp
	Commands/cmd_quest.cpp
	Commands/cmd_recall.cpp
	Commands/cmd_reload.cpp
	Commands/cmd_reset.cpp
	Commands/cmd_server.cpp
	Commands/cmd_ticket.cpp
	Commands/cmd_titles.cpp
	Commands/cmd_vehicle.cpp
	Commands/cmd_waypoint.cpp
	DBC/DBCStores.cpp
	AchievementMgr.cpp 
	CollideInterface.cpp  
	VoiceChatHandler.cpp 
	DayWatcherThread.cpp 
	CommonScheduleThread.cpp 
	ArenaTeam.cpp 
	DatabaseCleaner.cpp 
	ObjectStorage.cpp  
	AddonMgr.cpp 
	AIInterface.cpp 
	AIEvents.cpp
	AreaTrigger.cpp 
	Arenas.cpp 
	AuctionHouse.cpp 
	AuctionMgr.cpp 
	Battleground.cpp
	BattlegroundCommands.cpp 
	BattlegroundHandler.cpp 
	BattlegroundMgr.cpp 
	Channel.cpp 
	ChannelHandler.cpp 
	CharacterHandler.cpp 
	Chat.cpp 
	ChatHandler.cpp 
	CombatHandler.cpp 
	Container.cpp 
	Corpse.cpp 
	Creature.cpp 
	Entities/Summons/Summon.cpp
	Entities/Summons/CompanionSummon.cpp
	Entities/Summons/GuardianSummon.cpp
	Entities/Summons/PossessedSummon.cpp
	Entities/Summons/TotemSummon.cpp
	Entities/Summons/WildSummon.cpp
	debugcmds.cpp 
	DuelHandler.cpp 
	DynamicObject.cpp 
	EquipmentSetMgr.cpp
	EventableObject.cpp 
	EventMgr.cpp 
	Faction.cpp 
	GameObject.cpp 
	GMTicket.cpp 
	GMTicketCommands.cpp
	Gossip.cpp
	Group.cpp 
	GroupHandler.cpp 
	Guild.cpp 
	GuildHandler.cpp 
	HonorHandler.cpp 
	InstanceCommands.cpp 
	Item.cpp 
	ItemHandler.cpp 
	ItemInterface.cpp 
	Level0.cpp 
	Level1.cpp 
	Level2.cpp 
	Level3.cpp 
	LfgHandler.cpp 
	LfgMgr.cpp 
	LootMgr.cpp 
	LocalizationMgr.cpp 
	LogonCommClient.cpp 
	LogonCommHandler.cpp 
	MailSystem.cpp 
	MapCell.cpp 
	Map.cpp 
	MapScriptInterface.cpp 
	MapMgr.cpp 
	MiscHandler.cpp 
	MovementHandler.cpp 
	NPCHandler.cpp 
	Object.cpp 
	ObjectMgr.cpp 
	Opcodes.cpp 
	Pet.cpp 
	PetHandler.cpp 
	Player.cpp 
	PlayerPacketWrapper.cpp 
	QueryHandler.cpp 
	Quest.cpp 
	QuestHandler.cpp 
	QuestMgr.cpp 
	QuestCommands.cpp 
	RaidHandler.cpp 
	RecallCommands.cpp 
	ReputationHandler.cpp 
	ScriptMgr.cpp 
	SocialHandler.cpp 
	SpeedDetector.cpp 
	SpellAuras.cpp 
	SpellTarget.cpp 
	Spell.cpp 
	StdAfx.cpp
	SummonHandler.cpp
	SpellEffects.cpp 
	SpellHandler.cpp 
	HackFixes.cpp 
	Stats.cpp 
	TaxiHandler.cpp 
	TaxiMgr.cpp 
	TerrainMgr.cpp 
	TradeHandler.cpp 
	TransporterHandler.cpp 
	Unit.cpp 
	Vehicle.cpp 
	VehicleHandler.cpp
	WayPoints.cpp 
	WeatherMgr.cpp 
	World.cpp 
	WorldCreator.cpp 
	WorldSession.cpp 
	WorldSocket.cpp 
	WorldStatesHandler.cpp
	WorldRunnable.cpp 
	WordFilter.cpp 
	Main.cpp 
	Master.cpp 
	CConsole.cpp 
	ConsoleCommands.cpp 
	ConsoleListener.cpp 
	WUtil.cpp 
	SpellProc.cpp
	SpellProc_DeathKnight.cpp
	SpellProc_Druid.cpp
	SpellProc_Hunter.cpp
	SpellProc_Items.cpp
	SpellProc_Mage.cpp
	SpellProc_Paladin.cpp
	SpellProc_Priest.cpp
	SpellProc_Rogue.cpp
	SpellProc_Shaman.cpp
	SpellProc_Warlock.cpp
	SpellProc_Warrior.cpp
	SpellMgr.cpp
	Spell_DeathKnight.cpp
	Spell_Druid.cpp
	Spell_Hunter.cpp
	Spell_Mage.cpp
	Spell_Paladin.cpp
	Spell_Priest.cpp
	Spell_Rogue.cpp
	Spell_Shaman.cpp
	Spell_Warlock.cpp
	Spell_Warrior.cpp
	DeathKnight.cpp
	Druid.cpp
	Hunter.cpp
	Mage.cpp
	Paladin.cpp
	Priest.cpp
	Rogue.cpp
	Shaman.cpp
	Warlock.cpp
	Warrior.cpp
)

SET(headers
	AchievementMgr.h
	AddonMgr.h
	AIInterface.h
	AIEvents.h
	AreaTrigger.h
	Arenas.h
	ArenaTeam.h
	AuctionHouse.h
	AuctionMgr.h
	BaseConsole.h
	Battleground.h
	BattlegroundMgr.h
	CConsole.h
	CellHandler.h
	Channel.h
	ChannelMgr.h
	Chat.h
	CollideInterface.h
	CommonScheduleThread.h
	Entities/Summons/CompanionSummon.h
	ConsoleCommands.h
	Container.h
	Corpse.h
	Creature.h
	DatabaseCleaner.h
	DayWatcherThread.h
	DBC/DBCStores.h
	DeathKnight.h
	Definitions.h
	Druid.h
	DynamicObject.h
	EquipmentSetMgr.h
	Events.h
	EventableObject.h
	EventMgr.h
	Faction.h
	GameObject.h
	Gossip.h
	Group.h
	Entities/Summons/GuardianSummon.h
	Guild.h
	HonorHandler.h
	Hunter.h
	Item.h
	ItemInterface.h
	ItemPrototype.h
	LfgMgr.h
	LocalizationMgr.h
	LogonCommClient.h
	LogonCommHandler.h
	LootMgr.h
	Mage.h
	MailSystem.h
	MainServerDefines.h
	Map.h
	MapCell.h
	MapMgr.h
	MapScriptInterface.h
	Master.h
	MiscHandler.h
	NameTables.h
	NPCHandler.h
	Object.h
	ObjectMgr.h
	ObjectStorage.h
	Opcodes.h
	Paladin.h
	Pet.h
	Player.h
	PlayerCache.h
	Entities/Summons/PossessedSummon.h
	Priest.h
	Quest.h
	QuestMgr.h
	Rogue.h
	ScriptMgr.h
	ScriptSetup.h
	Shaman.h
	Skill.h
	SkillNameMgr.h
	SpeedDetector.h
	Spell.h
	SpellAuras.h
	SpellFailure.h
	SpellMgr.h
	SpellNameHashes.h
	SpellProc.h
	SpellTarget.h
	Stats.h
	StdAfx.h
	Entities/Summons/Summon.h
	SummonHandler.h
	TaxiMgr.h
	TerrainMgr.h
	Entities/Summons/TotemSummon.h
	TransporterHandler.h
	Unit.h
	UpdateFields.h
	UpdateMask.h
	Vehicle.h
	Warlock.h
	Warrior.h
	WeatherMgr.h
	Entities/Summons/WildSummon.h
	WordFilter.h
	World.h
	WorldCreator.h
	WorldRunnable.h
	WorldSession.h
	WorldSocket.h
	WorldStatesHandler.h
	WorldStates.h
	WUtil.h
)
	
FOREACH(src IN ITEMS ${sources})
  SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)
FOREACH(src IN ITEMS ${headers})
  SET(HEADERS ${HEADERS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE(PCHSupport)
INCLUDE_DIRECTORIES(${GLOBAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DEPENDENCY_LIBS} ${EXTRA_LIBS_PATH})

SET(WIN_RESOURCE_FILE ${ROOT_PATH}/src/arcemu-world/resources.rc)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS} ${HEADERS} ${WIN_RESOURCE_FILE})

IF(CMAKE_GENERATOR MATCHES Visual*)
	ADD_NATIVE_PRECOMPILED_HEADER(${PROJECT_NAME} StdAfx.h ${prefix}/StdAfx.cpp)
ENDIF()
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES ENABLE_EXPORTS TRUE)

ADD_DEPENDENCIES(${PROJECT_NAME} shared g3dlite vmaplib detour recast)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} shared  g3dlite vmaplib detour recast ${PCRE_LIBRARIES})

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)