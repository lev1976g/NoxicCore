/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008-2012 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

initialiseSingleton(ChatHandler);
initialiseSingleton(CommandTableStorage);

ChatCommand* ChatHandler::getCommandTable()
{
	ARCEMU_ASSERT(false);
	return 0;
}

ChatCommand* CommandTableStorage::GetSubCommandTable(const char* name)
{
	if(!stricmp(name, "modify"))
		return _modifyCommandTable;
	else if(!stricmp(name, "waypoint"))
		return _waypointCommandTable;
	else if(!stricmp(name, "debug"))
		return _debugCommandTable;
	else if(!stricmp(name, "gmTicket"))
		return _GMTicketCommandTable;
	else if(!stricmp(name, "gobject"))
		return _GameObjectCommandTable;
	else if(!stricmp(name, "battleground"))
		return _BattlegroundCommandTable;
	else if(!stricmp(name, "npc"))
		return _NPCCommandTable;
	else if(!stricmp(name, "cheat"))
		return _CheatCommandTable;
	else if(!stricmp(name, "account"))
		return _accountCommandTable;
	else if(!stricmp(name, "honor"))
		return _honorCommandTable;
	else if(!stricmp(name, "quest"))
		return _questCommandTable;
	else if(!stricmp(name, "pet"))
		return _petCommandTable;
	else if(!stricmp(name, "recall"))
		return _recallCommandTable;
	else if(!stricmp(name, "guild"))
		return _GuildCommandTable;
	else if(!stricmp(name, "gm"))
		return _gmCommandTable;
	else if(!stricmp(name, "server"))
		return _serverCommandTable;
	else if(!stricmp(name, "character"))
		return _characterCommandTable;
	else if(!stricmp(name, "lookup"))
		return _lookupCommandTable;
	else if(!stricmp(name, "admin"))
		return _adminCommandTable;
	else if(!stricmp(name, "kick"))
		return _kickCommandTable;
	else if(!stricmp(name, "ban"))
		return _banCommandTable;
	else if(!stricmp(name, "unban"))
		return _unbanCommandTable;
	else if(!stricmp(name, "instance"))
		return _instanceCommandTable;
	else if(!stricmp(name, "arena"))
		return _arenaCommandTable;
	else if(!stricmp(name, "achieve"))
		return _achievementCommandTable;
	else if(!stricmp(name, "vehicle"))
		return _vehicleCommandTable;
	return 0;
}

#define dupe_command_table(ct, dt) this->dt = (ChatCommand*)allocate_and_copy(sizeof(ct)/* / sizeof(ct[0])*/, ct)
ARCEMU_INLINE void* allocate_and_copy(uint32 len, void* pointer)
{
	void* data = (void*)malloc(len);
	memcpy(data, pointer, len);
	return data;
}

void CommandTableStorage::Load()
{
	Log.Notice("CommandTableStorage", "Loading command overrides...");
	QueryResult* result = WorldDatabase.Query("SELECT * FROM command_overrides");
	if(!result)
		return;

	do
	{
		const char* name = result->Fetch()[0].GetString();
		const char* level = result->Fetch()[1].GetString();
		Override(name, level);
	}
	while(result->NextRow());
	Log.Success("CommandTableStorage", "Loaded %u command overrides...", result->GetRowCount());
	delete result;
}

void CommandTableStorage::Override(const char* command, const char* level)
{
	ARCEMU_ASSERT(level[0] != '\0');
	char* cmd = strdup(command);

	// find the command we're talking about
	char* sp = strchr(cmd, ' ');
	const char* command_name = cmd;
	const char* subcommand_name = NULL;

	if(sp)
	{
		// we're dealing with a subcommand.
		*sp = 0;
		subcommand_name = sp + 1;
	}

	size_t len1 = strlen(command_name);
	size_t len2 = subcommand_name ? strlen(subcommand_name) : 0;

	// look for the command.
	ChatCommand* p = &_commandTable[0];
	while(p->Name != 0)
	{
		if(!strnicmp(p->Name, command_name, len1))
		{
			// this is the one we wanna modify
			if(!subcommand_name)
			{
				// no subcommand, we can change it.
				p->CommandGroup = level[0];
				LOG_DETAIL("Changing command level of command `%s` to %c.", p->Name, level[0]);
			}
			else
			{
				// assume this is a subcommand, loop the second set.
				ChatCommand* p2 = p->ChildCommands;
				if(!p2)
				{
					Log.Error("Chat.cpp", "Invalid command specified for override: %s", command_name);
				}
				else
				{
					while(p2->Name != 0)
					{
						if(!strnicmp("*", subcommand_name, 1))
						{
							p2->CommandGroup = level[0];
							LOG_DETAIL("Changing command level of command (wildcard) `%s`:`%s` to %c.", p->Name, p2->Name, level[0]);
						}
						else
						{
							if(!strnicmp(p2->Name, subcommand_name, len2))
							{
								// change the level
								p2->CommandGroup = level[0];
								LOG_DETAIL("Changing command level of command `%s`:`%s` to %c.", p->Name, p2->Name, level[0]);
								break;
							}
						}
						p2++;
					}
					if(!p2->Name)
					{
						if(strnicmp("*", subcommand_name, 1)) //Hacky.. meh.. -DGM
						{
							Log.Error("Chat.cpp", "Invalid subcommand referenced: `%s` under `%s`.", subcommand_name, p->Name);
						}
						break;
					}
				}
			}
			break;
		}
		++p;
	}

	if(!p->Name)
		Log.Error("Chat.cpp", "Invalid command referenced: `%s`", command_name);

	free(cmd);
}

void CommandTableStorage::Dealloc()
{
	free(_accountCommandTable);
	free(_achievementCommandTable);
	free(_adminCommandTable);
	free(_arenaCommandTable);
	free(_banCommandTable);
	free(_unbanCommandTable);
	free(_battlegroundCommandTable);
	free(_characterCommandTable);
	free(_kickCommandTable);
	free(_cheatCommandTable);
	free(_debugCommandTable);
	free(_eventCommandTable);
	free(_gmCommandTable);
	free(_goCommandTable);
	free(_gameobjectCommandTable);
	free(_groupCommandTable);
	free(_guildCommandTable);
	free(_honorCommandTable);
	free(_instanceCommandTable);
	free(_itemCommandTable);
	free(_learnCommandTable);
	free(_lfgCommandTable);
	free(_listCommandTable);
	free(_lookupCommandTable);
	free(_messageCommandTable);
	free(_miscCommandTable);
	free(_mmapsCommandTable);
	free(_modifyCommandTable);
	free(_npcCommandTable);
	free(_petCommandTable);
	free(_questCommandTable);
	free(_recallCommandTable);
	free(_reloadCommandTable);
	free(_resetCommandTable);
	free(_serverCommandTable);
	free(_ticketCommandTable);
	free(_titlesCommandTable);
	free(_vehicleCommandTable);
	free(_waypointCommandTable);
}

void CommandTableStorage::Init()
{
	/* account */
	static ChatCommand accountCommandTable[] =
	{
		{ "level",	'3', &ChatHandler::HandleAccountLevelCommand,	".account level <username> <0/1/2/3> - Sets gm level on account.",	NULL, 0, 0, 0 },
		{ "mute",   '3', &ChatHandler::HandleAccountMuteCommand,	".account mute <username> <timeperiod> - Mutes account.",			NULL, 0, 0, 0 },
		{ "unmute", '3', &ChatHandler::HandleAccountUnmuteCommand,	".account unmute <username> - Unmutes account",						NULL, 0, 0, 0 },
		{ NULL,     '0', NULL,										"",																	NULL, 0, 0, 0 }
	};
	dupe_command_table(accountCommandTable, _accountCommandTable);

	/* achievement */
	static ChatCommand achievementCommandTable[] =
	{
#ifdef ENABLE_ACHIEVEMENTS
		{ "complete", '2', &ChatHandler::HandleAchievementCompleteCommand, "Completes the specified achievement.",          NULL, 0, 0, 0 },
		{ "criteria", '2', &ChatHandler::HandleAchievementCriteriaCommand, "Completes the specified achievement criteria.", NULL, 0, 0, 0 },
		{ "reset",    '2', &ChatHandler::HandleAchievementResetCommand,    "Resets achievement data from the target.",      NULL, 0, 0, 0 },
#endif
		{ NULL,       '0', NULL,                                           "",                                              NULL, 0, 0, 0 }
	};
	dupe_command_table(achievementCommandTable, _achievementCommandTable);

	/* admin */
	static ChatCommand adminCommandTable[] =
	{
		{ "tag",					'3', &ChatHandler::HandleAdministratorCommand,		"Toggles the PLAYER_FLAG_ADMIN <Admin>",															NULL, 0, 0, 0 },
		{ "castall",				'3', &ChatHandler::HandleCastAllCommand,			"Makes all players online cast spell <x>.",															NULL, 0, 0, 0 },
		{ "dispelall",				'3', &ChatHandler::HandleDispelAllCommand,			"Dispels all negative (or positive w/ 1) auras on all players.",									NULL, 0, 0, 0 },
		{ "renameallinvalidchars",	'3', &ChatHandler::HandleRenameAllCharacter,		"Renames all invalid character names.",																NULL, 0, 0, 0 },
		{ "masssummon",				'3', &ChatHandler::HandleMassSummonCommand,			"Summons all online players to your location,add the a/A parameter for alliance or h/H for horde.",	NULL, 0, 0, 0 },
		{ "playall",				'3', &ChatHandler::HandleGlobalPlaySoundCommand,	"Plays a sound to everyone on the realm.",															NULL, 0, 0, 0 },
		{ NULL,						'0', NULL,											"",																									NULL, 0, 0, 0 }
	};
	dupe_command_table(adminCommandTable, _adminCommandTable);

	/* arena */
	static ChatCommand arenaCommandTable[] =
	{
		{ "createteam",			'1', &ChatHandler::HandleArenaCreateTeamCommand,		"Creates arena team.",								NULL, 0, 0, 0 },
		{ "setteamleader",		'1', &ChatHandler::HandleArenaSetTeamLeaderCommand,		"Sets the arena team leader.",						NULL, 0, 0, 0 },
		{ "resetallratings",	'1', &ChatHandler::HandleArenaResetAllRatingsCommand,	"Resets all arena teams to their default rating.",	NULL, 0, 0, 0 },
		{ NULL,					'0', NULL,												"",													NULL, 0, 0, 0 }
	};
	dupe_command_table(arenaCommandTable, _arenaCommandTable);

	/* ban */
	static ChatCommand banCommandTable[] =
	{
		{ "ip",			'3', &ChatHandler::HandleIPBanCommand,			"Adds an address to the IP ban table: .ban ip <address> [duration] [reason]\nDuration must be a number optionally followed by a character representing the calendar subdivision to use (h>hours, d>days, w>weeks, m>months, y>years, default minutes)\nLack of duration results in a permanent ban.",	NULL, 0, 0, 0 },
		{ "character",	'1', &ChatHandler::HandleBanCharacterCommand,	"Bans character: .ban character <char> [duration] [reason]",																																																											NULL, 0, 0, 0 },
		{ "account",	'2', &ChatHandler::HandleAccountBannedCommand,	"Bans account: .ban account <name> [duration] [reason]",																																																												NULL, 0, 0, 0 },
		{ "all",		'3', &ChatHandler::HandleBanAllCommand,			"Bans account, ip, and character: .ban all <char> [duration] [reason]",																																																									NULL, 0, 0, 0 },
		{ NULL,			'0', NULL,										"",																																																																										NULL, 0, 0, 0 }
	};
	dupe_command_table(banCommandTable, _banCommandTable);

	static ChatCommand unbanCommandTable[] =
	{
		{ "ip",			'3', &ChatHandler::HandleIPUnBanCommand,		"Deletes an address from the IP ban table: <address>",	NULL, 0, 0, 0 },
		{ "character",	'1', &ChatHandler::HandleUnBanCharacterCommand,	"Unbans character - <name>.",							NULL, 0, 0, 0 },
		{ "account",	'2', &ChatHandler::HandleAccountUnbanCommand,	"Unbans account - <username>.",							NULL, 0, 0, 0 },
		{ "all",		'3', &ChatHandler::HandleNYICommand,			"Unbans account, ip, and character.",					NULL, 0, 0, 0 },
		{ NULL,			'0', NULL,										"",														NULL, 0, 0, 0 }
	};
	dupe_command_table(unbanCommandTable, _unbanCommandTable);

	/* battleground */
	static ChatCommand battlegroundCommandTable[] =
	{
		{ "setbgscore",		'2', &ChatHandler::HandleSetBGScoreCommand,							"<Teamid> <Score> - Sets battleground score. 2 Arguments.",			NULL, 0, 0, 0 },
		{ "startbg",		'1', &ChatHandler::HandleStartBGCommand,							"Starts current battleground match.",								NULL, 0, 0, 0 },
		{ "pausebg",		'1', &ChatHandler::HandlePauseBGCommand,							"Pauses current battleground match.",								NULL, 0, 0, 0 },
		{ "bginfo",			'1', &ChatHandler::HandleBGInfoCommand,								"Displays information about current battleground.",					NULL, 0, 0, 0 },
		{ "battleground",	'1', &ChatHandler::HandleBattlegroundCommand,						"Shows BG Menu",													NULL, 0, 0, 0 },
		{ "setworldstate",	'3', &ChatHandler::HandleSetWorldStateCommand,						"<var> <val> - Var can be in hex. WS Value.",						NULL, 0, 0, 0 },
		{ "setworldstates",	'3', &ChatHandler::HandleSetWorldStatesCommand,						"<var> <val> - Var can be in hex. WS Value.",						NULL, 0, 0, 0 },
		{ "playsound",		'2', &ChatHandler::HandlePlaySoundCommand,							"<val>. Val can be in hex.",										NULL, 0, 0, 0 },
		{ "setbfstatus",	'3', &ChatHandler::HandleSetBattlefieldStatusCommand,				".setbfstatus - NYI.",												NULL, 0, 0, 0 },
		{ "leave",			'1', &ChatHandler::HandleBattlegroundExitCommand,					"Leaves the current battleground.",									NULL, 0, 0, 0 },
		{ "getqueue",		'1', &ChatHandler::HandleGetBattlegroundQueueCommand,				"Gets common battleground queue information.",						NULL, 0, 0, 0 },
		{ "forcestart",		'2', &ChatHandler::HandleInitializeAllQueuedBattlegroundsCommand,	"Forces initialization of all battlegrounds with active queue.",	NULL, 0, 0, 0 },
		{ NULL,				'0', NULL,															"",																	NULL, 0, 0, 0 }
	};
	dupe_command_table(battlegroundCommandTable, _battlegroundCommandTable);

	/* character */
	static ChatCommand characterCommandTable[] =
	{
		{ "learn",               'm', &ChatHandler::HandleLearnCommand,            "Learns spell",                                                                                                      NULL, 0, 0, 0 },
		{ "unlearn",             'm', &ChatHandler::HandleUnlearnCommand,          "Unlearns spell",                                                                                                    NULL, 0, 0, 0 },
		{ "getskillinfo",        'm', &ChatHandler::HandleGetSkillsInfoCommand,    "Gets all the skills from a player",                                                                                 NULL, 0, 0, 0 },
		{ "learnskill",          'm', &ChatHandler::HandleLearnSkillCommand,       ".learnskill <skillid> (optional) <value> <maxvalue> - Learns skill id skillid.",                                    NULL, 0, 0, 0 },
		{ "advanceskill",        'm', &ChatHandler::HandleModifySkillCommand,      "advanceskill <skillid> <amount, optional, default = 1> - Advances skill line x times..",                            NULL, 0, 0, 0 },
		{ "removeskill",         'm', &ChatHandler::HandleRemoveSkillCommand,      ".removeskill <skillid> - Removes skill",                                                                            NULL, 0, 0, 0 },
		{ "increaseweaponskill", 'm', &ChatHandler::HandleIncreaseWeaponSkill,     ".increaseweaponskill <count> - Increase equipped weapon skill x times (defaults to 1).",                             NULL, 0, 0, 0 },
		{ "resetreputation",     'n', &ChatHandler::HandleResetReputationCommand,  ".resetreputation - Resets reputation to start levels. (use on characters that were made before reputation fixes.)", NULL, 0, 0, 0 },
		{ "resetspells",         'n', &ChatHandler::HandleResetSpellsCommand,      ".resetspells - Resets all spells to starting spells of targeted player. DANGEROUS.",                                NULL, 0, 0, 0 },
		{ "resettalents",        'n', &ChatHandler::HandleResetTalentsCommand,     ".resettalents - Resets all talents of targeted player to that of their current level. DANGEROUS.",                  NULL, 0, 0, 0 },
		{ "resetskills",         'n', &ChatHandler::HandleResetSkillsCommand,      ".resetskills - Resets all skills.",                                                                                 NULL, 0, 0, 0 },
		{ "additem",             'm', &ChatHandler::HandleAddInvItemCommand,       "Adds item x count y",                                                                                                                  NULL, 0, 0, 0 },
		{ "removeitem",          'm', &ChatHandler::HandleRemoveItemCommand,       "Removes item x count y.",                                                                                         NULL, 0, 0, 0 },
		{ "additemset",          'm', &ChatHandler::HandleAddItemSetCommand,       "Adds item set to inv.",                                                                                             NULL, 0, 0, 0 },
		{ "advanceallskills",    'm', &ChatHandler::HandleAdvanceAllSkillsCommand, "Advances all skills <x> points.",                                                                                   NULL, 0, 0, 0 },
		{ "getstanding",         'm', &ChatHandler::HandleGetStandingCommand,      "Gets standing of faction x.",                                                                                      NULL, 0, 0, 0 },
		{ "setstanding",         'm', &ChatHandler::HandleSetStandingCommand,      "Sets standing of faction x.",                                                                                      NULL, 0, 0, 0 },
		{ "showitems",           'm', &ChatHandler::HandleShowItems,               "Shows items of selected Player",                                                                                    NULL, 0, 0, 0 },
		{ "showskills",          'm', &ChatHandler::HandleShowSkills,              "Shows skills of selected Player",                                                                                   NULL, 0, 0, 0 },
		{ "showinstances",       'z', &ChatHandler::HandleShowInstancesCommand,    "Shows persistent instances of selected Player",                                                                     NULL, 0, 0, 0 },
		{ "rename",              'm', &ChatHandler::HandleRenameCommand,           "Renames character x to y.",                                                                                         NULL, 0, 0, 0 },
		{ "forcerename",         'm', &ChatHandler::HandleForceRenameCommand,      "Forces character x to rename his char next login",                                                                  NULL, 0, 0, 0 },
		{ "repairitems",         'n', &ChatHandler::HandleRepairItemsCommand,      ".repairitems - Repair all items from selected player",                                                              NULL, 0, 0, 0 },
		{ "settitle",			 'm', &ChatHandler::HandleSetTitle,				   "Adds title to a player",																					NULL, 0, 0, 0 },
		{ "phase",               'm', &ChatHandler::HandlePhaseCommand,            "<phase> - Sets phase of selected player",                                                                           NULL, 0, 0, 0 },
		{ NULL,                  '0', NULL,                                        "",                                                                                                                  NULL, 0, 0, 0 }
	};
	dupe_command_table(characterCommandTable, _characterCommandTable);

	/* cheat */
	static ChatCommand cheatCommandTable[] =
	{
		{ "status",      'm', &ChatHandler::HandleShowCheatsCommand,       "Shows active cheats.",                            NULL, 0, 0, 0 },
		{ "taxi",        'm', &ChatHandler::HandleTaxiCheatCommand,        "Enables all taxi nodes.",                         NULL, 0, 0, 0 },
		{ "cooldown",    'm', &ChatHandler::HandleCooldownCheatCommand,    "Enables no cooldown cheat.",                      NULL, 0, 0, 0 },
		{ "casttime",    'm', &ChatHandler::HandleCastTimeCheatCommand,    "Enables no cast time cheat.",                     NULL, 0, 0, 0 },
		{ "power",       'm', &ChatHandler::HandlePowerCheatCommand,       "Disables mana consumption etc.",                  NULL, 0, 0, 0 },
		{ "god",         'm', &ChatHandler::HandleGodModeCommand,          "Sets god mode, prevents you from taking damage.", NULL, 0, 0, 0 },
		{ "fly",         'm', &ChatHandler::HandleFlyCommand,              "Sets fly mode",                                   NULL, 0, 0, 0 },
		{ "explore",     'm', &ChatHandler::HandleExploreCheatCommand,     "Reveals the unexplored parts of the map.",        NULL, 0, 0, 0 },
		{ "flyspeed",    'm', &ChatHandler::HandleModifySpeedCommand,      "Does the same thing as .modify speed",            NULL, 0, 0, 0 },
		{ "stack",       'm', &ChatHandler::HandleAuraStackCheatCommand,   "Enables aura stacking cheat.",                    NULL, 0, 0, 0 },
		{ "itemstack",   'm', &ChatHandler::HandleItemStackCheatCommand,   "Enables item stacking cheat.",                    NULL, 0, 0, 0 },
		{ "triggerpass", 'm', &ChatHandler::HandleTriggerpassCheatCommand, "Ignores area trigger prerequisites.",             NULL, 0, 0, 0 },
		{ NULL,          '0', NULL,                                        "",                                                NULL, 0, 0, 0 }
	};
	dupe_command_table(cheatCommandTable, _cheatCommandTable);

	/* debug */
	static ChatCommand debugCommandTable[] =
	{
		{ "infront",             'd', &ChatHandler::HandleDebugInFrontCommand,     "",                                                                                                                  NULL, 0, 0, 0 },
		{ "showreact",           'd', &ChatHandler::HandleShowReactionCommand,     "",                                                                                                                  NULL, 0, 0, 0 },
		{ "aimove",              'd', &ChatHandler::HandleAIMoveCommand,           "",                                                                                                                  NULL, 0, 0, 0 },
		{ "dist",                'd', &ChatHandler::HandleDistanceCommand,         "",                                                                                                                  NULL, 0, 0, 0 },
		{ "face",                'd', &ChatHandler::HandleFaceCommand,             "",                                                                                                                  NULL, 0, 0, 0 },
		{ "moveinfo",            'd', &ChatHandler::HandleMoveInfoCommand,         "",                                                                                                                  NULL, 0, 0, 0 },
		{ "setbytes",            'd', &ChatHandler::HandleSetBytesCommand,         "",                                                                                                                  NULL, 0, 0, 0 },
		{ "getbytes",            'd', &ChatHandler::HandleGetBytesCommand,         "",                                                                                                                  NULL, 0, 0, 0 },
		{ "root",                'd', &ChatHandler::HandleDebugRoot,               "",                                                                                                                  NULL, 0, 0, 0 },
		{ "landwalk",            'd', &ChatHandler::HandleDebugLandWalk,           "",                                                                                                                  NULL, 0, 0, 0 },
		{ "waterwalk",           'd', &ChatHandler::HandleDebugWaterWalk,          "",                                                                                                                  NULL, 0, 0, 0 },
		{ "castspell",           'd', &ChatHandler::HandleCastSpellCommand,        ".castspell <spellid> - Casts spell on target.",                                                                     NULL, 0, 0, 0 },
		{ "castself",            'd', &ChatHandler::HandleCastSelfCommand,         ".castself <spellId> - Target casts spell <spellId> on itself.",                                                     NULL, 0, 0, 0 },
		{ "castspellne",         'd', &ChatHandler::HandleCastSpellNECommand,      ".castspellne <spellid> - Casts spell on target (only plays animations, doesn't handle effects or range/facing/etc.", NULL, 0, 0, 0 },
		{ "aggrorange",          'd', &ChatHandler::HandleAggroRangeCommand,       ".aggrorange - Shows aggro Range of the selected Creature.",                                                         NULL, 0, 0, 0 },
		{ "knockback",           'd', &ChatHandler::HandleKnockBackCommand,        ".knockback <value> - Knocks you back.",                                                                             NULL, 0, 0, 0 },
		{ "fade",                'd', &ChatHandler::HandleFadeCommand,             ".fade <value> - calls ModThreatModifyer().",                                                                        NULL, 0, 0, 0 },
		{ "threatMod",           'd', &ChatHandler::HandleThreatModCommand,        ".threatMod <value> - calls ModGeneratedThreatModifyer().",                                                          NULL, 0, 0, 0 },
		{ "calcThreat",          'd', &ChatHandler::HandleCalcThreatCommand,       ".calcThreat <dmg> <spellId> - calculates threat.",                                                                  NULL, 0, 0, 0 },
		{ "threatList",          'd', &ChatHandler::HandleThreatListCommand,       ".threatList  - returns all AI_Targets of the selected Creature.",                                                   NULL, 0, 0, 0 },
		{ "gettptime",           'd', &ChatHandler::HandleGetTransporterTime,      "grabs transporter travel time",                                                                                     NULL, 0, 0, 0 },
		{ "itempushresult",      'd', &ChatHandler::HandleSendItemPushResult,      "sends item push result",                                                                                            NULL, 0, 0, 0 },
		{ "setbit",              'd', &ChatHandler::HandleModifyBitCommand,        "",                                                                                                                  NULL, 0, 0, 0 },
		{ "setvalue",            'd', &ChatHandler::HandleModifyValueCommand,      "",                                                                                                                  NULL, 0, 0, 0 },
		{ "aispelltestbegin",    'd', &ChatHandler::HandleAIAgentDebugBegin,       "",                                                                                                                  NULL, 0, 0, 0 },
		{ "aispelltestcontinue", 'd', &ChatHandler::HandleAIAgentDebugContinue,    "",                                                                                                                  NULL, 0, 0, 0 },
		{ "aispelltestskip",     'd', &ChatHandler::HandleAIAgentDebugSkip,        "",                                                                                                                  NULL, 0, 0, 0 },
		{ "dumpcoords",          'd', &ChatHandler::HandleDebugDumpCoordsCommmand, "",                                                                                                                  NULL, 0, 0, 0 },
		{ "sendpacket",          'd', &ChatHandler::HandleSendpacket,              "<opcode ID>, <data>",                                                                                               NULL, 0, 0, 0 },
		{ "sqlquery",            'd', &ChatHandler::HandleSQLQueryCommand,         "<sql query>",                                                                                                       NULL, 0, 0, 0 },
		{ "rangecheck",          'd', &ChatHandler::HandleRangeCheckCommand,       "Checks the 'yard' range and internal range between the player and the target.",                                     NULL, 0, 0, 0 },
		{ "setallratings",       'd', &ChatHandler::HandleRatingsCommand,          "Sets rating values to incremental numbers based on their index.",                                                   NULL, 0, 0, 0 },
		{ "testlos",             'd', &ChatHandler::HandleCollisionTestLOS,        "tests los",                                                                                                         NULL, 0, 0, 0 },
		{ "testindoor",          'd', &ChatHandler::HandleCollisionTestIndoor,     "tests indoor",                                                                                                      NULL, 0, 0, 0 },
		{ "getheight",           'd', &ChatHandler::HandleCollisionGetHeight,      "Gets height",                                                                                                       NULL, 0, 0, 0 },
		{ "deathstate",          'd', &ChatHandler::HandleGetDeathState,           "returns current deathstate for target",                                                                             NULL, 0, 0, 0 },
		{ "getpos",              'd', &ChatHandler::HandleGetPosCommand,           "",                                                                                                                  NULL, 0, 0, 0 },
		{ "sendfailed",			 'd', &ChatHandler::HandleSendFailed,      "",                                                                                                                  NULL, 0, 0, 0 },
		{ "playmovie",			 'd', &ChatHandler::HandlePlayMovie,			   "Triggers a movie for a player",									NULL, 0, 0, 0 },
		{ "auraupdate",			 'd', &ChatHandler::HandleAuraUpdateAdd,			   "<SpellID> <Flags> <StackCount> (caster guid = player target)",									NULL, 0, 0, 0 },
		{ "auraremove",			 'd', &ChatHandler::HandleAuraUpdateRemove,			   "<VisualSlot>",									NULL, 0, 0, 0 },
		{ "spawnwar",			 'd', &ChatHandler::HandleDebugSpawnWarCommand,	   "Spawns desired amount of npcs to fight with eachother",																NULL, 0, 0, 0 },
		{ "updateworldstate",    'd', &ChatHandler::HandleUpdateWorldStateCommand, "Sets the specified worldstate field to the specified value",                                                        NULL, 0, 0, 0 },
		{ "initworldstates",     'd', &ChatHandler::HandleInitWorldStatesCommand,  "(re)initializes the worldstates.",                                                                                  NULL, 0, 0, 0 },
		{ "clearworldstates",    'd', &ChatHandler::HandleClearWorldStatesCommand, "Clears the worldstates",                                                                                            NULL, 0, 0, 0 },
		{ NULL,                  '0', NULL,                                        "",                                                                                                                  NULL, 0, 0, 0 }
	};
	dupe_command_table(debugCommandTable, _debugCommandTable);

	/* event */
	static ChatCommand eventCommandTable[] =
	{
		{ "spawn",		'3', &ChatHandler::HandleNYICommand, ".event spawn <ID> \n Description: Spawns an event.",		NULL, 0, 0, 0 }
		{ "despawn",	'3', &ChatHandler::HandleNYICommand, ".event despawn <ID> \n Description: Despawns an event.",	NULL, 0, 0, 0 }
		{ NULL,			'0', NULL, "",																					NULL, 0, 0, 0 }
	};
	dupe_command_table(eventCommandTable, _eventCommandTable);

	/* gamemaster */
	static ChatCommand gmCommandTable[] =
	{
		{ "fly",         'm', &ChatHandler::HandleFlyCommand,              "Sets fly mode",                                   NULL, 0, 0, 0 },
		{ "list",          '0', &ChatHandler::HandleGMListCommand,        "Shows active GM's",                                   NULL, 0, 0, 0 },
		{ "status",        't', &ChatHandler::HandleGMStatusCommand,      "Shows status of your gm flags",             	    	 NULL, 0, 0, 0 },
		{ "off",           't', &ChatHandler::HandleGMOffCommand,         "Sets GM tag off",                                     NULL, 0, 0, 0 },
		{ "on",            't', &ChatHandler::HandleGMOnCommand,          "Sets GM tag on",                                      NULL, 0, 0, 0 },
		{ "whisperblock",  'g', &ChatHandler::HandleWhisperBlockCommand,  "Blocks like .gmon except without the <GM> tag",       NULL, 0, 0, 0 },
		{ "allowwhispers", 'c', &ChatHandler::HandleAllowWhispersCommand, "Allows whispers from player <s> while in gmon mode.", NULL, 0, 0, 0 },
		{ "blockwhispers", 'c', &ChatHandler::HandleBlockWhispersCommand, "Blocks whispers from player <s> while in gmon mode.", NULL, 0, 0, 0 },
		{ NULL,            '0', NULL,                                     "",                                                    NULL, 0, 0, 0 }
	};
	dupe_command_table(gmCommandTable, _gmCommandTable);

	static ChatCommand kickCommandTable[] =
	{
		{ "player",  'f', &ChatHandler::HandleKillByPlayerCommand,  "Disconnects the player with name <s>.",          NULL, 0, 0, 0 },
		{ "account", 'f', &ChatHandler::HandleKillBySessionCommand, "Disconnects the session with account name <s>.", NULL, 0, 0, 0 },
		{ "ip",      'f', &ChatHandler::HandleKillByIPCommand,      "Disconnects the session with the ip <s>.",       NULL, 0, 0, 0 },
		{ NULL,        '0', NULL,                                     "",                                               NULL, 0, 0, 0 }
	};
	dupe_command_table(kickCommandTable, _kickCommandTable);

	/* go */
	static ChatCommand goCommandTable[] =
	{
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(goCommandTable, _goCommandTable);

	/* gobject */
	static ChatCommand gameobjectCommandTable[] =
	{
		{ "select",       'o', &ChatHandler::HandleGOSelect,       "Selects the nearest GameObject to you",    NULL, 0, 0, 0 },
		{ "delete",       'o', &ChatHandler::HandleGODelete,       "Deletes selected GameObject",              NULL, 0, 0, 0 },
		{ "spawn",        'o', &ChatHandler::HandleGOSpawn,        "Spawns a GameObject by ID",                NULL, 0, 0, 0 },
		{ "phase",        'o', &ChatHandler::HandleGOPhaseCommand, "<phase> <save> - Phase selected GameObject", NULL, 0, 0, 0 },
		{ "info",         'o', &ChatHandler::HandleGOInfo,         "Gives you information about selected GO", NULL, 0, 0, 0 },
		{ "damage",       'o', &ChatHandler::HandleGODamageCommand,"Damages the GO for the specified hitpoints",NULL, 0, 0, 0},
		{ "rebuild",      'o', &ChatHandler::HandleGORebuildCommand,"Rebuilds the GO.",                        NULL, 0, 0, 0 },
		{ "activate",     'o', &ChatHandler::HandleGOActivate,     "Activates/Opens the selected GO.",         NULL, 0, 0, 0 },
		{ "enable",       'o', &ChatHandler::HandleGOEnable,       "Enables the selected GO for use.",         NULL, 0, 0, 0 },
		{ "scale",        'o', &ChatHandler::HandleGOScale,        "Sets scale of selected GO",                NULL, 0, 0, 0 },
		{ "animprogress", 'o', &ChatHandler::HandleGOAnimProgress, "Sets anim progress",                       NULL, 0, 0, 0 },
		{ "faction",      'o', &ChatHandler::HandleGOFactionCommand,"Sets the faction of the GO",              NULL, 0, 0, 0 },
		{ "export",       'o', &ChatHandler::HandleGOExport,       "Exports the current GO selected",          NULL, 0, 0, 0 },
		{ "move",         'g', &ChatHandler::HandleGOMove,         "Moves gameobject to player xyz",           NULL, 0, 0, 0 },
		{ "rotate",       'g', &ChatHandler::HandleGORotate,       "<Axis> <Value> - Rotates the object. <Axis> x,y, Default o.",             NULL, 0, 0, 0 },
		{ "portto",       'v', &ChatHandler::HandlePortToGameObjectSpawnCommand, "Teleports you to the gameobject with spawn id x.", NULL, 0, 0, 0 },
		{ NULL,           '0', NULL,                               "",                                         NULL, 0, 0, 0 }
	};
	dupe_command_table(gameobjectCommandTable, _gameobjectCommandTable);

	/* group */
	static ChatCommand groupCommandTable[] =
	{
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(groupCommandTable, _groupCommandTable);

	/* guild */
	static ChatCommand guildCommandTable[] =
	{
		{ "join",         'm', &ChatHandler::HandleGuildJoinCommand,         "Force joins a guild",                 NULL, 0, 0, 0 },
		{ "create",       'm', &ChatHandler::/*Handle*/CreateGuildCommand,             "Creates a guild.",                    NULL, 0, 0, 0 },
		{ "rename",       'm', &ChatHandler::HandleRenameGuildCommand,       "Renames a guild.",                    NULL, 0, 0, 0 },
		{ "members",      'm', &ChatHandler::HandleGuildMembersCommand,      "Lists guildmembers and their ranks.", NULL, 0, 0, 0 },
		{ "removeplayer", 'm', &ChatHandler::HandleGuildRemovePlayerCommand, "Removes a player from a guild.",      NULL, 0, 0, 0 },
		{ "disband",      'm', &ChatHandler::HandleGuildDisbandCommand,      "Disbands the guild of your target.",  NULL, 0, 0, 0 },
		{ NULL,           '0', NULL,                                         "",                                    NULL, 0, 0, 0 }
	};
	dupe_command_table(guildCommandTable, _guildCommandTable);

	/* honor */
	static ChatCommand honorCommandTable[] =
	{
		{ "addpoints",         'm', &ChatHandler::HandleAddHonorCommand,                    "Adds x amount of honor points/currency",                  NULL, 0, 0, 0 },
		{ "addkills",          'm', &ChatHandler::HandleAddKillCommand,                     "Adds x amount of honor kills",                            NULL, 0, 0, 0 },
		{ "globaldailyupdate", 'm', &ChatHandler::HandleGlobalHonorDailyMaintenanceCommand, "Daily honor field moves",                                 NULL, 0, 0, 0 },
		{ "singledailyupdate", 'm', &ChatHandler::HandleNextDayCommand,                     "Daily honor field moves for selected player only",        NULL, 0, 0, 0 },
		{ "pvpcredit",         'm', &ChatHandler::HandlePVPCreditCommand,                   "Sends PVP credit packet, with specified rank and points", NULL, 0, 0, 0 },
		{ NULL,                '0', NULL,                                                   "",                                                        NULL, 0, 0, 0 }
	};
	dupe_command_table(honorCommandTable, _honorCommandTable);

	/* instance */
	static ChatCommand instanceCommandTable[] =
	{
		{ "create",   'z', &ChatHandler::HandleCreateInstanceCommand,    "Generically instances a map that requires instancing, mapid x y z",		      NULL, 0, 0, 0 },
		{ "reset",    'z', &ChatHandler::HandleResetInstanceCommand,     "Removes instance ID x from target player.",                         NULL, 0, 0, 0 },
		{ "resetall", 'm', &ChatHandler::HandleResetAllInstancesCommand, "Removes all instance IDs from target player.",                      NULL, 0, 0, 0 },
		{ "show",       'z', &ChatHandler::HandleShowInstancesCommand,    "Shows persistent instances of selected Player",                                                                     NULL, 0, 0, 0 },
		{ "shutdown", 'z', &ChatHandler::HandleShutdownInstanceCommand,  "Shutdown instance with ID x (default is current instance).",        NULL, 0, 0, 0 },
		//{ "delete",   'z', &ChatHandler::HandleDeleteInstanceCommand,    "Deletes instance with ID x (default is current instance).",         NULL, 0, 0, 0 },
		{ "info",     'm', &ChatHandler::HandleGetInstanceInfoCommand,   "Gets info about instance with ID x (default is current instance).", NULL, 0, 0, 0 },
		{ "exit",     'm', &ChatHandler::HandleExitInstanceCommand,      "Exits current instance, return to entry point.",                    NULL, 0, 0, 0 },
		{ NULL,       '0', NULL,                                         "",                                                                  NULL, 0, 0, 0 }
	};
	dupe_command_table(instanceCommandTable, _instanceCommandTable);

	/* item */
	static ChatCommand itemCommandTable[] =
	{
		{ "add", 'm', &ChatHandler::HandleAddInvItemCommand, "Adds item x count y", NULL, 0, 0, 0 },
		{ "remove", 'm', &ChatHandler::HandleRemoveItemCommand, "Removes item x count y.", NULL, 0, 0, 0 },
		{ "addset", 'm', &ChatHandler::HandleAddItemSetCommand, "Adds item set to inv.", NULL, 0, 0, 0 },
		{ "repair", 'n', &ChatHandler::HandleRepairItemsCommand, ".repairitems - Repair all items from selected player", NULL, 0, 0, 0 },
		{ "show", 'm', &ChatHandler::HandleShowItems, "Shows items of selected Player", NULL, 0, 0, 0 },
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(itemCommandTable, _itemCommandTable);

	/* learn */
	static ChatCommand learnCommandTable[] =
	{
		{ "spell", 'm', &ChatHandler::HandleLearnCommand, "Learns spell", NULL, 0, 0, 0 },
		{ "skill", 'm', &ChatHandler::HandleLearnSkillCommand, ".learnskill <skillid> (optional) <value> <maxvalue> - Learns skill id skillid.", NULL, 0, 0, 0 },
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(learnCommandTable, _learnCommandTable);

	/* lfg */
	static ChatCommand lfgCommandTable[] =
	{
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(lfgCommandTable, _lfgCommandTable);

	/* list */
	static ChatCommand listCommandTable[] =
	{
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(listCommandTable, _listCommandTable);

	/* lookup */
	static ChatCommand lookupCommandTable[] =
	{
		{ "item",     'l', &ChatHandler::HandleLookupItemCommand,     "Looks up item string x.",  NULL, 0, 0, 0 },
		{ "quest",    'l', &ChatHandler::HandleQuestLookupCommand,    "Looks up quest string x.", NULL, 0, 0, 0 },
		{ "creature", 'l', &ChatHandler::HandleLookupCreatureCommand, "Looks up item string x.",  NULL, 0, 0, 0 },
		{ "object",   'l', &ChatHandler::HandleLookupObjectCommand,   "Looks up gameobject string x.", NULL, 0, 0 , 0},
		{ "spell",    'l', &ChatHandler::HandleLookupSpellCommand,    "Looks up spell string x.", NULL, 0, 0, 0 },
		{ "skill",    'l', &ChatHandler::HandleLookupSkillCommand,    "Looks up skill string x.", NULL, 0, 0, 0 },
		{ "faction",  'l', &ChatHandler::HandleLookupFactionCommand,  "Looks up faction string x.", NULL, 0, 0, 0 },
#ifdef ENABLE_ACHIEVEMENTS
		{ "achievement", 'l', &ChatHandler::HandleLookupAchievementCmd,  "Looks up achievement string x.", NULL, 0, 0, 0 },
#endif
		{ NULL,          '0', NULL,                                      "",                               NULL, 0, 0, 0 }
	};
	dupe_command_table(lookupCommandTable, _lookupCommandTable);

	/* message */
	static ChatCommand messageCommandTable[] =
	{
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(messageCommandTable, _messageCommandTable);

	/* mmaps */
	static ChatCommand mmapsCommandTable[] =
	{
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(mmapsCommandTable, _mmapsCommandTable);

	/* modify */
	static ChatCommand modifyCommandTable[] =
	{
		{ "hp",					'm', NULL,                                   "Modifies health points (HP) of selected target",                  NULL, UNIT_FIELD_HEALTH,                 UNIT_FIELD_MAXHEALTH, 1 },
		{ "gender",				'm', &ChatHandler::HandleGenderChanger,      "Changes gender of selected target. Usage: 0=male, 1=female.",     NULL, 0,                                 0,                    0 },
		{ "mana",				'm', NULL,                                   "Modifies mana points (MP) of selected target.",                   NULL, UNIT_FIELD_POWER1,                 UNIT_FIELD_MAXPOWER1, 1 },
		{ "rage",				'm', NULL,                                   "Modifies rage points of selected target.",                        NULL, UNIT_FIELD_POWER2,                 UNIT_FIELD_MAXPOWER2, 1 },
		{ "energy",				'm', NULL,                                   "Modifies energy points of selected target.",                      NULL, UNIT_FIELD_POWER4,                 UNIT_FIELD_MAXPOWER4, 1 },
		{ "runicpower",			'm', NULL,                                   "Modifies runic power points of selected target.",                 NULL, UNIT_FIELD_POWER7,                 UNIT_FIELD_MAXPOWER7, 1 },
		{ "level",				'm', &ChatHandler::HandleModifyLevelCommand, "Modifies the level of selected target.",                          NULL, 0,                                 0,                    0 },
		{ "strength",			'm', NULL,                                   "Modifies the strength value of the selected target.",             NULL, UNIT_FIELD_STAT0,                  0,                    1 },
		{ "agility",			'm', NULL,                                   "Modifies the agility value of the selected target.",              NULL, UNIT_FIELD_STAT1,                  0,                    1 },
		{ "intelligence",		'm', NULL,                                   "Modifies the intelligence value of the selected target.",         NULL, UNIT_FIELD_STAT3,                  0,                    1 },
		{ "spirit",				'm', NULL,                                   "Modifies the spirit value of the selected target.",               NULL, UNIT_FIELD_STAT4,                  0,                    1 },
		{ "armor",				'm', NULL,                                   "Modifies the armor of selected target.",                          NULL, UNIT_FIELD_RESISTANCES,            0,                    1 },
		{ "holy",				'm', NULL,                                   "Modifies the holy resistance of selected target.",                NULL, UNIT_FIELD_RESISTANCES + 1,         0,                    1 },
		{ "fire",				'm', NULL,                                   "Modifies the fire resistance of selected target.",                NULL, UNIT_FIELD_RESISTANCES + 2,         0,                    1 },
		{ "nature",				'm', NULL,                                   "Modifies the nature resistance of selected target.",              NULL, UNIT_FIELD_RESISTANCES + 3,         0,                    1 },
		{ "frost",				'm', NULL,                                   "Modifies the frost resistance of selected target.",               NULL, UNIT_FIELD_RESISTANCES + 4,         0,                    1 },
		{ "shadow",				'm', NULL,                                   "Modifies the shadow resistance of selected target.",              NULL, UNIT_FIELD_RESISTANCES + 5,         0,                    1 },
		{ "arcane",				'm', NULL,                                   "Modifies the arcane resistance of selected target.",              NULL, UNIT_FIELD_RESISTANCES + 6,         0,                    1 },
		{ "damage",				'm', NULL,                                   "Modifies the damage done by the selected target.",                NULL, UNIT_FIELD_MINDAMAGE,              UNIT_FIELD_MAXDAMAGE, 2 },
		{ "ap",					'm', NULL,                                   "Modifies the attack power of the selected target.",               NULL, UNIT_FIELD_ATTACK_POWER,           0,                    1 },
		{ "rangeap",			'm', NULL,                                   "Modifies the range attack power of the selected target.",         NULL, UNIT_FIELD_RANGED_ATTACK_POWER,    0,                    1 },
		{ "scale",				'm', NULL,                                   "Modifies the scale of the selected target.",                      NULL, OBJECT_FIELD_SCALE_X,              0,                    2 },
		{ "gold",				'm', &ChatHandler::HandleModifyGoldCommand,  "Modifies the gold amount of the selected target. Copper value.",  NULL, 0,                                 0,                    0 },
		{ "flyspeed",    'm', &ChatHandler::HandleModifySpeedCommand,      "Does the same thing as .modify speed",            NULL, 0, 0, 0 },
		{ "speed",				'm', &ChatHandler::HandleModifySpeedCommand, "Modifies the movement speed of the selected target.",             NULL, 0,                                 0,                    0 },
		{ "nativedisplayid",	'm', NULL,                                   "Modifies the native display identifier of the target.",           NULL, UNIT_FIELD_NATIVEDISPLAYID,        0,                    1 },
		{ "display",			'm', NULL,                                   "Modifies the display identifier (DisplayID) of the target.",      NULL, UNIT_FIELD_DISPLAYID,              0,                    1 },
		{ "displayid",			'm', NULL,                                   "Modifies the display identifier (DisplayID) of the target.",      NULL, UNIT_FIELD_DISPLAYID,              0,                    1 },
		{ "flags",				'm', NULL,                                   "Modifies the flags of the selected target.",                      NULL, UNIT_FIELD_FLAGS,                  0,                    1 },
		{ "faction",			'm', NULL,                                   "Modifies the faction template of the selected target.",           NULL, UNIT_FIELD_FACTIONTEMPLATE,        0,                    1 },
		{ "dynamicflags",		'm', NULL,                                   "Modifies the dynamic flags of the selected target.",              NULL, UNIT_DYNAMIC_FLAGS,                0,                    1 },
		{ "talentpoints",		'm', &ChatHandler::HandleModifyTPsCommand,	  "Modifies the available talent points of the selected target.",    NULL, 0,								  0,                    0 },
		{ "happiness",			'm', NULL,                                   "Modifies the happiness value of the selected target.",            NULL, UNIT_FIELD_POWER5,                 UNIT_FIELD_MAXPOWER5, 1 },
		{ "boundingraidius",	'm', NULL,                                   "Modifies the bounding radius of the selected target.",            NULL, UNIT_FIELD_BOUNDINGRADIUS,         0,                    2 },
		{ "combatreach",		'm', NULL,                                   "Modifies the combat reach of the selected target.",               NULL, UNIT_FIELD_COMBATREACH,            0,                    2 },
		{ "npcemotestate",		'm', NULL,                                   "Modifies the NPC emote state of the selected target.",            NULL, UNIT_NPC_EMOTESTATE,               0,                    1 },
		{ "bytes0",				'm', NULL,                                   "WARNING! Modifies the bytes0 entry of selected target.",          NULL, UNIT_FIELD_BYTES_0,                0,                    1 },
		{ "bytes1",				'm', NULL,                                   "WARNING! Modifies the bytes1 entry of selected target.",          NULL, UNIT_FIELD_BYTES_1,                0,                    1 },
		{ "bytes2",				'm', NULL,                                   "WARNING! Modifies the bytes2 entry of selected target.",          NULL, UNIT_FIELD_BYTES_2,                0,                    1 },
		{ NULL,					'0', NULL,                                   "",                                                                NULL, 0,                                 0,                    0 }
	};
	dupe_command_table(modifyCommandTable, _modifyCommandTable);

	/* npc */
	static ChatCommand npcCommandTable[] =
	{
		{ "vendoradditem",    'n', &ChatHandler::HandleItemCommand,           "Adds to vendor",                                                                                                                          NULL, 0, 0, 0 },
		{ "vendorremoveitem", 'n', &ChatHandler::HandleItemRemoveCommand,     "Removes from vendor.",                                                                                                                    NULL, 0, 0, 0 },
		{ "flags",            'n', &ChatHandler::HandleNPCFlagCommand,        "Changes NPC flags",                                                                                                                       NULL, 0, 0, 0 },
		{ "emote",            'n', &ChatHandler::HandleEmoteCommand,          ".emote - Sets emote state",                                                                                                               NULL, 0, 0, 0 },
		{ "delete",           'n', &ChatHandler::HandleDeleteCommand,         "Deletes mob from db and world.",                                                                                                          NULL, 0, 0, 0 },
		{ "info",             'n', &ChatHandler::HandleNpcInfoCommand,        "Displays NPC information",                                                                                                                NULL, 0, 0, 0 },
		{ "phase",            'n', &ChatHandler::HandleCreaturePhaseCommand,  "<phase> <save> - Sets phase of selected mob",                                                                                             NULL, 0, 0, 0 },
		{ "addAgent",         'n', &ChatHandler::HandleAddAIAgentCommand,     ".npc addAgent <agent> <procEvent> <procChance> <procCount> <spellId> <spellType> <spelltargetType> <spellCooldown> <floatMisc1> <Misc2>", NULL, 0, 0, 0 },
		{ "listAgent",        'n', &ChatHandler::HandleListAIAgentCommand,    ".npc listAgent",                                                                                                                          NULL, 0, 0, 0 },
		{ "say",              'n', &ChatHandler::HandleMonsterSayCommand,     ".npc say <text> - Makes selected mob say text <text>.",                                                                                   NULL, 0, 0, 0 },
		{ "yell",             'n', &ChatHandler::HandleMonsterYellCommand,    ".npc yell <Text> - Makes selected mob yell text <text>.",                                                                                 NULL, 0, 0, 0 },
		{ "come",             'n', &ChatHandler::HandleNpcComeCommand,        ".npc come - Makes npc move to your position",                                                                                             NULL, 0, 0, 0 },
		{ "return",           'n', &ChatHandler::HandleNpcReturnCommand,      ".npc return - Returns ncp to spawnpoint.",                                                                                                NULL, 0, 0, 0 },
		{ "spawn",            'n', &ChatHandler::HandleCreatureSpawnCommand,  ".npc spawn - Spawns npc of entry <id>",                                                                                                   NULL, 0, 0, 0 },
		{ "respawn",          'n', &ChatHandler::HandleCreatureRespawnCommand, ".respawn - Respawns a dead npc from its corpse.",                                                                                         NULL, 0, 0, 0 },
		{ "spawnlink",        'n', &ChatHandler::HandleNpcSpawnLinkCommand,   ".spawnlink sqlentry",                                                                                                                     NULL, 0, 0, 0 },
		{ "possess",          'n', &ChatHandler::HandleNpcPossessCommand,     ".npc possess - Possess an npc (mind control)",                                                                                            NULL, 0, 0, 0 },
		{ "unpossess",        'n', &ChatHandler::HandleNpcUnPossessCommand,   ".npc unpossess - Unpossess any currently possessed npc.",                                                                                 NULL, 0, 0, 0 },
		{ "select",           'n', &ChatHandler::HandleNpcSelectCommand,      ".npc select - selects npc closest",                                                                                                       NULL, 0, 0, 0 },
		{ "npcfollow",        'm', &ChatHandler::HandleNpcFollowCommand,      "Sets npc to follow you",                                                                                                                  NULL, 0, 0, 0 },
		{ "nullfollow",       'm', &ChatHandler::HandleNullFollowCommand,     "Sets npc to not follow anything",                                                                                                         NULL, 0, 0, 0 },
		{ "formationlink1",   'm', &ChatHandler::HandleFormationLink1Command, "Sets formation master.",                                                                                                                  NULL, 0, 0, 0 },
		{ "formationlink2",   'm', &ChatHandler::HandleFormationLink2Command, "Sets formation slave with distance and angle",                                                                                            NULL, 0, 0, 0 },
		{ "formationclear",   'm', &ChatHandler::HandleFormationClearCommand, "Removes formation from creature",                                                                                                         NULL, 0, 0, 0 },
		{ "equip1",           'm', &ChatHandler::HandleNPCEquipOneCommand,    "Use: .npc equip1 <itemid> - use .npc equip1 0 to remove the item",                                                                        NULL, 0, 0, 0 },
		{ "equip2",           'm', &ChatHandler::HandleNPCEquipTwoCommand,    "Use: .npc equip2 <itemid> - use .npc equip2 0 to remove the item",                                                                        NULL, 0, 0, 0 },
		{ "equip3",           'm', &ChatHandler::HandleNPCEquipThreeCommand,  "Use: .npc equip3 <itemid> - use .npc equip3 0 to remove the item",                                                                        NULL, 0, 0, 0 },
		{ "portto",           'v', &ChatHandler::HandlePortToCreatureSpawnCommand, "Teleports you to the creature with spawn id x.",                                                                                     NULL, 0, 0, 0 },
		{ "loot",             'm', &ChatHandler::HandleNPCLootCommand,        ".npc loot <quality> - displays possible loot for the selected NPC.",                                                                      NULL, 0, 0, 0 },
		{ "canfly",           'n', &ChatHandler::HandleNPCCanFlyCommand,      ".npc canfly <save> - Toggles CanFly state",                                                                                                      NULL, 0, 0, 0 },
		{ "ongameobject",     'n', &ChatHandler::HandleNPCOnGOCommand,        ".npc ongameobject <save> - Toggles onGameobject state. Required when spawning a NPC on a Gameobject",                                            NULL, 0, 0, 0 },
		{ "cast",             'n', &ChatHandler::HandleNPCCastCommand,        ".npc cast < spellid > - Makes the NPC cast this spell.",																							NULL, 0, 0, 0 },
		{ NULL,               '0', NULL,                                      "",                                                                                                                                        NULL, 0, 0, 0 }
	};
	dupe_command_table(npcCommandTable, _npcCommandTable);

	/* pet */
	static ChatCommand petCommandTable[] =
	{
		{ "createpet",   'm', &ChatHandler::HandleCreatePetCommand,      "Creates a pet with <entry>.",                            NULL, 0, 0, 0 },
		{ "dismiss",     'm', &ChatHandler::HandleDismissPetCommand,     "Dismisses selected pet.",                                NULL, 0, 0, 0 },
		{ "renamepet",   'm', &ChatHandler::HandleRenamePetCommand,      "Renames a pet to <name>.",                               NULL, 0, 0, 0 },
		{ "addspell",    'm', &ChatHandler::HandleAddPetSpellCommand,    "Teaches pet <spell>.",                                   NULL, 0, 0, 0 },
		{ "removespell", 'm', &ChatHandler::HandleRemovePetSpellCommand, "Removes pet spell <spell>.",                             NULL, 0, 0, 0 },
		{ "setlevel",    'm', &ChatHandler::HandlePetLevelCommand,       "Sets pet level to <level>.",                             NULL, 0, 0, 0 },
#ifdef USE_SPECIFIC_AIAGENTS
		{ "spawnbot",    'a', &ChatHandler::HandlePetSpawnAIBot,         ".pet spawnbot <type> - spawn a helper bot for your aid", NULL, 0, 0, 0 },
#endif
		{ NULL,          '0', NULL,                                      "",                                                       NULL, 0, 0, 0 }
	};
	dupe_command_table(petCommandTable, _petCommandTable);

	/* quest */
	static ChatCommand questCommandTable[] =
	{
		{ "addboth",   '2', &ChatHandler::HandleQuestAddBothCommand,   "Add quest <id> to the targeted NPC as start & finish",      NULL, 0, 0, 0 },
		{ "addfinish", '2', &ChatHandler::HandleQuestAddFinishCommand, "Add quest <id> to the targeted NPC as finisher",            NULL, 0, 0, 0 },
		{ "addstart",  '2', &ChatHandler::HandleQuestAddStartCommand,  "Add quest <id> to the targeted NPC as starter",             NULL, 0, 0, 0 },
		{ "delboth",   '2', &ChatHandler::HandleQuestDelBothCommand,   "Delete quest <id> from the targeted NPC as start & finish", NULL, 0, 0, 0 },
		{ "delfinish", '2', &ChatHandler::HandleQuestDelFinishCommand, "Delete quest <id> from the targeted NPC as finisher",       NULL, 0, 0, 0 },
		{ "delstart",  '2', &ChatHandler::HandleQuestDelStartCommand,  "Delete quest <id> from the targeted NPC as starter",        NULL, 0, 0, 0 },
		{ "complete",  '2', &ChatHandler::HandleQuestFinishCommand,    "Complete/Finish quest <id>",                                NULL, 0, 0, 0 },
		{ "fail",      '2', &ChatHandler::HandleQuestFailCommand,      "Fail quest <id>",                                           NULL, 0, 0, 0 },
		{ "finisher",  '2', &ChatHandler::HandleQuestFinisherCommand,  "Lookup quest finisher for quest <id>",                      NULL, 0, 0, 0 },
		{ "item",      '2', &ChatHandler::HandleQuestItemCommand,      "Lookup itemid necessary for quest <id>",                    NULL, 0, 0, 0 },
		{ "list",      '2', &ChatHandler::HandleQuestListCommand,      "Lists the quests for the npc <id>",                         NULL, 0, 0, 0 },
		{ "load",      '2', &ChatHandler::HandleQuestLoadCommand,      "Loads quests from database",                                NULL, 0, 0, 0 },
		{ "lookup",    '2', &ChatHandler::HandleQuestLookupCommand,    "Looks up quest string x",                                   NULL, 0, 0, 0 },
		{ "giver",     '2', &ChatHandler::HandleQuestGiverCommand,     "Lookup quest giver for quest <id>",                         NULL, 0, 0, 0 },
		{ "remove",    '2', &ChatHandler::HandleQuestRemoveCommand,    "Removes the quest <id> from the targeted player",           NULL, 0, 0, 0 },
		{ "reward",    '2', &ChatHandler::HandleQuestRewardCommand,    "Shows reward for quest <id>",                               NULL, 0, 0, 0 },
		{ "status",    '2', &ChatHandler::HandleQuestStatusCommand,    "Lists the status of quest <id>",                            NULL, 0, 0, 0 },
		{ "start",     '2', &ChatHandler::HandleQuestStartCommand,     "Starts quest <id>",                                         NULL, 0, 0, 0 },
		{ "startspawn", '2', &ChatHandler::HandleQuestStarterSpawnCommand, "Port to spawn location for quest <id> (starter)",        NULL, 0, 0, 0 },
		{ "finishspawn", '2', &ChatHandler::HandleQuestFinisherSpawnCommand, "Port to spawn location for quest <id> (finisher)",       NULL, 0, 0, 0 },
		{ NULL,        '0', NULL,                                      "",                                                          NULL, 0, 0, 0 }
	};
	dupe_command_table(questCommandTable, _questCommandTable);

	/* recall */
	static ChatCommand recallCommandTable[] =
	{
		{ "list",       'q', &ChatHandler::HandleRecallListCommand,       "List recall locations",     NULL, 0, 0, 0 },
		{ "add",        'q', &ChatHandler::HandleRecallAddCommand,        "Add a recall location",       NULL, 0, 0, 0 },
		{ "del",        'q', &ChatHandler::HandleRecallDelCommand,        "Remove a recall location",  NULL, 0, 0, 0 },
		{ "port",       'q', &ChatHandler::HandleRecallGoCommand,         "Ports you to recalled location", NULL, 0, 0, 0 },
		{ "portplayer", 'm', &ChatHandler::HandleRecallPortPlayerCommand, "Ports specified player to a recalled location", NULL, 0, 0, 0 },
		{ "portus",		'm', &ChatHandler::HandleRecallPortUsCommand,	  "Ports you and the selected player to recalled location",       NULL, 0, 0, 0 },
		{ NULL,         '0', NULL,                                        "",                          NULL, 0, 0, 0 }
	};
	dupe_command_table(recallCommandTable, _recallCommandTable);

	/* reload */
	static ChatCommand reloadCommandTable[] =
	{
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(reloadCommandTable, _reloadCommandTable);

	/* reset */
	static ChatCommand resetCommandTable[] =
	{
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(resetCommandTable, _resetCommandTable);

	/* server */
	static ChatCommand serverCommandTable[] =
	{
		{ "setmotd",       'm', &ChatHandler::HandleSetMotdCommand,         "Sets MOTD",                                                NULL, 0, 0, 0 },
		{ "rehash",        'z', &ChatHandler::HandleRehashCommand,          "Reloads config file.",                                     NULL, 0, 0, 0 },
		//{ "reloadscripts", 'w', &ChatHandler::HandleReloadScriptsCommand,   "Reloads GM Scripts",                                       NULL, 0, 0, 0 },
		{ "reloadtable",   'm', &ChatHandler::HandleDBReloadCommand,        "",                      NULL, 0, 0, 0 },
		{ "shutdown",      'z', &ChatHandler::HandleShutdownCommand,        "Initiates server shutdown in <x> seconds (5 by default).", NULL, 0, 0, 0 },
		{ "restart",       'z', &ChatHandler::HandleShutdownRestartCommand, "Initiates server restart in <x> seconds (5 by default).",  NULL, 0, 0, 0 },
		{ "cancelshutdown", 'z', &ChatHandler::HandleCancelShutdownCommand,  "Cancels a Server Restart/Shutdown.",						NULL, 0, 0, 0 },
		{ "save",          's', &ChatHandler::HandleSaveCommand,            "Save's target character",									NULL, 0, 0, 0 },
		{ "saveall",       's', &ChatHandler::HandleSaveAllCommand,         "Save's all playing characters",                            NULL, 0, 0, 0 },
		{ "info",          '0', &ChatHandler::HandleInfoCommand,            "Server info",                                              NULL, 0, 0, 0 },
		{ "netstatus",     '0', &ChatHandler::HandleNetworkStatusCommand,   "Shows network status.",									NULL, 0, 0, 0 },
		{ "scriptsreload", '0', &ChatHandler::HandleReloadScripts,          "Reloads script engines.",									NULL, 0, 0, 0 },
 		{ "reloadscripts", '0', &ChatHandler::HandleReloadScripts,          "Reloads script engines.",									NULL, 0, 0, 0 },
 		{ NULL,            '0', NULL,                                       "",                                                         NULL, 0, 0, 0 }
	};
	dupe_command_table(serverCommandTable, _serverCommandTable);

	/* ticket */
	static ChatCommand ticketCommandTable[] =
	{
#ifdef GM_TICKET_MY_MASTER_COMPATIBLE
		{ "get",             '1', &ChatHandler::HandleGMTicketListCommand,                     "Gets GM Ticket list.",                                          NULL, 0, 0, 0 },
		{ "getId",           '1', &ChatHandler::HandleGMTicketGetByIdCommand,                  "Gets GM Ticket by player name.",                                NULL, 0, 0, 0 },
		{ "delId",           '1', &ChatHandler::HandleGMTicketRemoveByIdCommand,               "Deletes GM Ticket by player name.",                             NULL, 0, 0, 0 },
#else
		{ "list",            '1', &ChatHandler::HandleGMTicketListCommand,                     "Lists all active GM Tickets.",                                  NULL, 0, 0, 0 },
		{ "get",             '1', &ChatHandler::HandleGMTicketGetByIdCommand,                  "Gets GM Ticket with ID x.",                                     NULL, 0, 0, 0 },
		{ "remove",          '1', &ChatHandler::HandleGMTicketRemoveByIdCommand,               "Removes GM Ticket with ID x.",                                  NULL, 0, 0, 0 },
		{ "deletepermanent", '3', &ChatHandler::HandleGMTicketDeletePermanentCommand,          "Deletes GM Ticket with ID x permanently.",                      NULL, 0, 0, 0 },
		{ "assign",          '2', &ChatHandler::HandleGMTicketAssignToCommand,                 "Assigns GM Ticket with id x to GM y (if empty to your self).", NULL, 0, 0, 0 },
		{ "release",         '1', &ChatHandler::HandleGMTicketReleaseCommand,                  "Releases assigned GM Ticket with ID x.",                        NULL, 0, 0, 0 },
		{ "comment",         '1', &ChatHandler::HandleGMTicketCommentCommand,                  "Sets comment x to GM Ticket with ID y.",                        NULL, 0, 0, 0 },
#endif
		{ "toggle",          '3', &ChatHandler::HandleGMTicketToggleTicketSystemStatusCommand, "Toggles the ticket system status.",                             NULL, 0, 0, 0 },
		{ NULL,              '0', NULL,                                                        "",                                                              NULL, 0, 0, 0 }
	};
	dupe_command_table(ticketCommandTable, _ticketCommandTable);

	/* titles */
	static ChatCommand titlesCommandTable[] =
	{
		{ "set", 'm', &ChatHandler::HandleSetTitle, "Adds title to a player", NULL, 0, 0, 0 },
		{ NULL, '0', NULL, "", NULL, 0, 0, 0 }
	};
	dupe_command_table(titlesCommandTable, _titlesCommandTable);

	/* vehicle */
	static ChatCommand vehicleCommandTable[] =
	{
		{ "ejectpassenger",       '2', &ChatHandler::HandleVehicleEjectPassengerCommand,     "Ejects the passenger from the specified seat",      NULL, 0, 0, 0 },
		{ "ejectallpassengers",   '2', &ChatHandler::HandleVehicleEjectAllPassengersCommand, "Ejects all passengers from the vehicle",            NULL, 0, 0, 0 },
		{ "installaccessories",   '2', &ChatHandler::HandleVehicleInstallAccessoriesCommand, "Installs the accessories for the selected vehicle", NULL, 0, 0, 0 },
		{ "removeaccessories",    '2', &ChatHandler::HandleVehicleRemoveAccessoriesCommand,  "Removes the accessories of the selected vehicle",   NULL, 0, 0, 0 },
		{ "addpassenger",         '2', &ChatHandler::HandleVehicleAddPassengerCommand,       "Adds a new NPC passenger to the vehicle",           NULL, 0, 0, 0 },
		{ NULL,                   '0', NULL,                                                 "",                                                  NULL, 0, 0, 0 }
	};
	dupe_command_table(vehicleCommandTable, _vehicleCommandTable);

	/* waypoint */
	static ChatCommand waypointCommandTable[] =
	{
		{ "add",       '2', &ChatHandler::HandleWPAddCommand,          "Add wp at current pos",  NULL, 0, 0, 0 },
		{ "show",      '2', &ChatHandler::HandleWPShowCommand,         "Show wp's for creature", NULL, 0, 0, 0 },
		{ "hide",      '2', &ChatHandler::HandleWPHideCommand,         "Hide wp's for creature", NULL, 0, 0, 0 },
		{ "delete",    '2', &ChatHandler::HandleWPDeleteCommand,       "Delete selected wp",     NULL, 0, 0, 0 },
		{ "movehere",  '2', &ChatHandler::HandleWPMoveHereCommand,     "Move to this wp",        NULL, 0, 0, 0 },
		{ "flags",     '2', &ChatHandler::HandleWPFlagsCommand,        "Wp flags",               NULL, 0, 0, 0 },
		{ "waittime",  '2', &ChatHandler::HandleWPWaitCommand,         "Wait time at this wp",   NULL, 0, 0, 0 },
		{ "emote",     '2', &ChatHandler::HandleWPEmoteCommand,        "Emote at this wp",       NULL, 0, 0, 0 },
		{ "skin",      '2', &ChatHandler::HandleWPSkinCommand,         "Skin at this wp",        NULL, 0, 0, 0 },
		{ "change",    '2', &ChatHandler::HandleWPChangeNoCommand,     "Change at this wp",      NULL, 0, 0, 0 },
		{ "info",      '2', &ChatHandler::HandleWPInfoCommand,         "Show info for wp",       NULL, 0, 0, 0 },
		{ "movetype",  '2', &ChatHandler::HandleWPMoveTypeCommand,     "Movement type at wp",    NULL, 0, 0, 0 },
		{ "generate",  '2', &ChatHandler::HandleGenerateWaypoints,     "Randomly generate wps",  NULL, 0, 0, 0 },
		{ "save",      '2', &ChatHandler::HandleSaveWaypoints,         "Save all waypoints",     NULL, 0, 0, 0 },
		{ "deleteall", '2', &ChatHandler::HandleDeleteWaypoints,       "Delete all waypoints",   NULL, 0, 0, 0 },
		{ "addfly",    '2', &ChatHandler::HandleWaypointAddFlyCommand, "Adds a flying waypoint", NULL, 0, 0, 0 },
		{ NULL,        '0', NULL,                                      "",                       NULL, 0, 0, 0 }
	};
	dupe_command_table(waypointCommandTable, _waypointCommandTable);

	/* misc */
	static ChatCommand miscCommandTable[] =
	{
		{ "account",			'3', NULL,														"Account command table.",																													accountCommandTable,		0, 0, 0 },
		{ "achieve",			'2', NULL,														"Achievement command table.",																												achievementCommandTable,	0, 0, 0 },
		{ "achievement",		'2', NULL,														"Achievement command table.",																												achievementCommandTable,	0, 0, 0 },
		{ "additem",			'2', &ChatHandler::HandleAddInvItemCommand,						"Adds a item with an optional specific amount.",																							NULL,						0, 0, 0 },
		{ "additemset",			'2', &ChatHandler::HandleAddItemSetCommand,						"Adds the items part of a set.",																											NULL,						0, 0, 0 },
		{ "addtrainerspell",	'3', &ChatHandler::HandleAddTrainerSpellCommand,				"Adds a spell to the trainer.",																												NULL,						0, 0, 0 },
		{ "admin",				'3', NULL,														"Administrator command table.",																												adminCommandTable,			0, 0, 0 },
		{ "administrator",		'3', NULL,														"Administrator command table.",																												adminCommandTable,			0, 0, 0 },
		{ "announce",			'1', &ChatHandler::HandleAnnounceCommand,						"Sends a normal chat message broadcast to all players.",																					NULL,						0, 0, 0 },
		{ "appear",				'1', &ChatHandler::HandleAppearCommand,							"Teleports to x's position.",																												NULL,						0, 0, 0 },
		{ "arena",				'1', NULL,														"Arena command table.",																														arenaCommandTable,			0, 0, 0 },
		{ "aura",				'1', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "ban",				'2', NULL,														"Ban command table.",																														banCommandTable,			0, 0, 0 },
		{ "bank",				'1', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "battleground",		'1', NULL,														"Battleground command table.",																												battlegroundCommandTable,	0, 0, 0 },
		{ "bindsight",			'1', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "calcdist",			'0', &ChatHandler::HandleSimpleDistanceCommand,					"Display the distance between your current position and the specified point x y z",															NULL,						0, 0, 0 },
		{ "character",			'1', NULL,														"Character command table.",																													characterCommandTable,		0, 0, 0 },
		{ "cheat",				'1', NULL,														"Cheat command table.",																														cheatCommandTable,			0, 0, 0 },
		{ "clearcooldowns",		'1', &ChatHandler::HandleClearCooldownsCommand,					"Clears all cooldowns for your class.",																										NULL,						0, 0, 0 },
		{ "cometome",			'3', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "commands",			'0', &ChatHandler::HandleCommandsCommand,						"Shows the available commands.",																											NULL,						0, 0, 0 },
		{ "cooldown",			'2', &ChatHandler::HandleCooldownCommand,						"Clears all cooldowns.",																													NULL,						0, 0, 0 },
		{ "damage",				'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "debug",				'0', NULL,														"Debug command table.",																														debugCommandTable,			0, 0, 0 },
		{ "demorph",			'1', &ChatHandler::HandleDeMorphCommand,						"Demorphs from morphed model.",																												NULL,						0, 0, 0 },
		{ "dev",				'2', &ChatHandler::HandleDeveloperCommand,						".dev <on/off> - Toggles the PLAYER_FLAG_DEVELOPER (<Dev>) tag.",																			NULL,						0, 0, 0 },
		{ "devtag",				'1', &ChatHandler::HandleDeveloperCommand,						".devtag <on/off> - Toggles the PLAYER_FLAG_DEVELOPER (<Dev>) tag.",																		NULL,						0, 0, 0 },
		{ "die",				'2', &ChatHandler::HandleKillCommand,							"Kills the selected target.",																												NULL,						0, 0, 0 },
		{ "dismount",			'1', &ChatHandler::HandleDismountCommand,						"Dismounts you from your mount.",																											NULL,						0, 0, 0 },
		{ "distance",			'2', &ChatHandler::HandleSimpleDistanceCommand,					"",																																			NULL,						0, 0, 0 },
		{ "fixscale",			'3', &ChatHandler::HandleFixScaleCommand,						"",																																			NULL,						0, 0, 0 },
		{ "freeze",				'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "gamemaster",			'0', NULL,														"Gamemaster command table.",																												gmCommandTable,				0, 0, 0 },
		{ "gameobject",			'0', NULL,														"GameObject command table.",																												gameobjectCommandTable,		0, 0, 0 },
		{ "gm",					'0', NULL,														"Gamemaster command table.",																												gmCommandTable,				0, 0, 0 },
		{ "gmannounce",			'1', &ChatHandler::HandleGMAnnounceCommand,						"Sends Msg to all online GMs",																												NULL,						0, 0, 0 },
		{ "gmticket",			'0', NULL,														"Gamemaster Ticket command table.",																											ticketCommandTable,			0, 0, 0 },
		{ "gobject",			'0', NULL,														"GameObject command table.",																												gameobjectCommandTable,		0, 0, 0 },
		{ "go",					'0', NULL,														"GameObject command table.",																												gameobjectCommandTable,		0, 0, 0 },
		{ "gotrig",				'1', &ChatHandler::HandleTriggerCommand,						"Warps to areatrigger <id>",																												NULL,						0, 0, 0 },
		{ "gps",				'0', &ChatHandler::HandleGPSCommand,							"Shows current position.",																													NULL,						0, 0, 0 },
		{ "guild",				'0', NULL,														"Guild command table.",																														guildCommandTable,			0, 0, 0 },
		{ "guid",				'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "help",				'0', &ChatHandler::HandleHelpCommand,							"Shows help for command.",																													NULL,						0, 0, 0 },
		{ "hidearea",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "honor",				'0', NULL,														"Honor command table.",																														honorCommandTable,			0, 0, 0 },
		{ "instance",			'0', NULL,														"Instance command table.",																													instanceCommandTable,		0, 0, 0 },
		{ "invincible",			'1', &ChatHandler::HandleInvincibleCommand,						".invincible - Toggles INVINCIBILITY (mobs won't attack you)",																				NULL,						0, 0, 0 },
		{ "invisible",			'1', &ChatHandler::HandleInvisibleCommand,						".invisible - Toggles INVINCIBILITY and INVISIBILITY (mobs won't attack you and nobody can see you, but they can see your chat messages)",	NULL,						0, 0, 0 },
		{ "itemmove",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "kill",				'2', &ChatHandler::HandleKillCommand,							"Kills the selected unit.",																													NULL,						0, 0, 0 },
		{ "killplr",			'2', &ChatHandler::HandleKillByPlrCommand,						".killplr [name] - Kills the specified or selected player.",																				NULL,						0, 0, 0 },
		{ "kick",				'2', NULL,														"Kick command table.",																														kickCommandTable,			0, 0, 0 },
		{ "kickplayer",			'2', &ChatHandler::HandleKickCommand,							"Kicks player from server",																													NULL,						0, 0, 0 },
		{ "levelup",			'2', &ChatHandler::HandleLevelUpCommand,						"Levelup x lvls",																															NULL,						0, 0, 0 },
		{ "linkgrave",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "listfreeze",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "logcomment",			'1', &ChatHandler::HandleGmLogCommentCommand,					"Adds a comment to the GM log for the administrators to read.",																				NULL,						0, 0, 0 },
		{ "lookup",				'0', NULL,														"Lookup command table.",																													lookupCommandTable,			0, 0, 0 },
		{ "mailbox",			'1', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "maxskill",			'1', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "modify",				'0', NULL,														"Modify command table.",																													modifyCommandTable,			0, 0, 0 },
		{ "modperiod",			'3', &ChatHandler::HandleModPeriodCommand,						"Changes period of current transporter.",																									NULL,						0, 0, 0 },
		{ "mount",				'3', &ChatHandler::HandleMountCommand,							"Mounts into modelid x.",																													NULL,						0, 0, 0 },
		{ "movegens",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "mute",				'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "neargrave",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "npc",				'0', NULL,														"NPC command table.",																														npcCommandTable,			0, 0, 0 },
		{ "paralyze",			'1', &ChatHandler::HandleParalyzeCommand,						"Roots/Paralyzes the target.",																												NULL,						0, 0, 0 },
		{ "playerinfo",			'3', &ChatHandler::HandlePlayerInfo,							".playerinfo - Displays information about the selected character (account...)",																NULL,						0, 0, 0 },
		{ "pet",				'0', NULL,														"Pet command table.",																														petCommandTable,			0, 0, 0 },
		{ "pinfo",				'3', &ChatHandler::HandlePlayerInfo,							"",																																			NULL,						0, 0, 0 },
		{ "playall",			'3', &ChatHandler::HandleGlobalPlaySoundCommand,				"",																																			NULL,						0, 0, 0 },
		{ "possess",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "quest",				'0', NULL,														"Quest command table.",																														questCommandTable,			0, 0, 0 },
		{ "recall",				'0', NULL,														"Recall command table.",																													recallCommandTable,			0, 0, 0 },
		{ "removeauras",		'm', &ChatHandler::HandleRemoveAurasCommand,					"Removes all auras from target",																											NULL,						0, 0, 0 },
		{ "removesickness",		'm', &ChatHandler::HandleRemoveRessurectionSickessAuraCommand,	"Removes ressurrection sickness from the target",																							NULL,						0, 0, 0 },
		{ "repairitems",		'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "respawn",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "revive",				'1', &ChatHandler::HandleReviveCommand,							"Revives the selected target. If no target is selected, the command user is selected by default.",											NULL,						0, 0, 0 },
		{ "reviveplr",			'1', &ChatHandler::HandleReviveStringCommand,					"Revives the specific player.",																												NULL,						0, 0, 0 },
		{ "root",				'1', &ChatHandler::HandleDebugRoot,								"Roots/Paralyzes the target.",																												NULL,						0, 0, 0 },
		{ "saveall",			'3', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "save",				'3', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "server",				'0', NULL,														"Server command table.",																													serverCommandTable,			0, 0, 0 },
		{ "setskill",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "showarea",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "start",				'1', &ChatHandler::HandleStartCommand,							"Teleports you to a starting location",																										NULL,						0, 0, 0 },
		{ "summon",				'1', &ChatHandler::HandleSummonCommand,							"Summons x to your position.",																												NULL,						0, 0, 0 },
		{ "ticket",				'0', NULL,														"Gamemaster Ticket command table.",																											ticketCommandTable,			0, 0, 0 },
		{ "unaura",				'1', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "unban",				'0', NULL,														"Unban command table.",																														unbanCommandTable,			0, 0, 0 },
		{ "unbindsight",		'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "unfreeze",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "unmute",				'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		//{ "unparalyze",			'b', &ChatHandler::HandleUnParalyzeCommand,						"Unroots/Unparalyzes the target.",																											NULL,						0, 0, 0 },
		{ "unpossess",			'2', NULL,														"",																																			NULL,						0, 0, 0 },
		//{ "unroot",				'b', &ChatHandler::HandleUnParalyzeCommand,						"Unroots/Unparalyzes the target.",																											NULL,						0, 0, 0 },
		{ "unstuck",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ "vehicle",			'3', NULL,														"Vehicle command table.",																													vehicleCommandTable,		0, 0, 0 },
		{ "waypoint",			'0', NULL,														"Waypoint command table.",																													waypointCommandTable,		0, 0, 0 },
		{ "worldport",			'1', &ChatHandler::HandleWorldPortCommand,						"Teleports you to a location with mapid x y z",																								NULL,						0, 0, 0 },
		{ "wp",					'0', NULL,														"Waypoint command table.",																													waypointCommandTable,		0, 0, 0 },
		{ "wannounce",			'1', &ChatHandler::HandleWAnnounceCommand,						"Sends a widescreen raid style announcement to all players.",																				NULL,						0, 0, 0 },
		{ "wchange",			'2', &ChatHandler::HandleNYICommand,							"",																																			NULL,						0, 0, 0 },
		{ NULL,					'0', NULL,														"",																																			NULL,						0, 0, 0 }
	};
	dupe_command_table(miscCommandTable, _miscCommandTable);

	/* set the correct pointers */
	ChatCommand* p = &_commandTable[0];
	while(p->Name != 0)
	{
		if(p->ChildCommands != 0)
		{
			// Set the correct pointer.
			ChatCommand* np = GetSubCommandTable(p->Name);
			ARCEMU_ASSERT(np != NULL);
			p->ChildCommands = np;
		}
		++p;
	}
}

ChatHandler::ChatHandler()
{
	new CommandTableStorage;
	CommandTableStorage::getSingleton().Init();
	SkillNameManager = new SkillNameMgr;
}

ChatHandler::~ChatHandler()
{
	CommandTableStorage::getSingleton().Dealloc();
	delete CommandTableStorage::getSingletonPtr();
	delete SkillNameManager;
}

bool ChatHandler::hasStringAbbr(const char* s1, const char* s2)
{
	for(;;)
	{
		if(!*s2)
			return true;
		else if(!*s1)
			return false;
		else if(tolower(*s1) != tolower(*s2))
			return false;
		s1++;
		s2++;
	}
}

void ChatHandler::SendMultilineMessage(WorldSession* m_session, const char* str)
{
	char* start = (char*)str, *end;
	for(;;)
	{
		end = strchr(start, '\n');
		if(!end)
			break;

		*end = '\0';
		SystemMessage(m_session, start);
		start = end + 1;
	}
	if(*start != '\0')
		SystemMessage(m_session, start);
}

bool ChatHandler::ExecuteCommandInTable(ChatCommand* table, const char* text, WorldSession* m_session)
{
	std::string cmd = "";

	// get command
	while(*text != ' ' && *text != '\0')
	{
		cmd += *text;
		text++;
	}

	while(*text == ' ') text++;  // skip whitespace

	if(!cmd.length())
		return false;

	for(uint32 i = 0; table[i].Name != NULL; i++)
	{
		if(!hasStringAbbr(table[i].Name, cmd.c_str()))
			continue;

		if(table[i].CommandGroup != '0' && !m_session->CanUseCommand(table[i].CommandGroup))
			continue;

		if(table[i].ChildCommands != NULL)
		{
			if(!ExecuteCommandInTable(table[i].ChildCommands, text, m_session))
			{
				if(table[i].Help != "")
					SendMultilineMessage(m_session, table[i].Help.c_str());
				else
				{
					GreenSystemMessage(m_session, "Available Subcommands:");
					for(uint32 k = 0; table[i].ChildCommands[k].Name; k++)
					{
						if(table[i].ChildCommands[k].CommandGroup == '0' || (table[i].ChildCommands[k].CommandGroup != '0' && m_session->CanUseCommand(table[i].ChildCommands[k].CommandGroup)))
							BlueSystemMessage(m_session, " %s - %s", table[i].ChildCommands[k].Name, table[i].ChildCommands[k].Help.size() ? table[i].ChildCommands[k].Help.c_str() : "No Help Available");
					}
				}
			}

			return true;
		}

		// Check for field-based commands
		if(table[i].Handler == NULL && (table[i].MaxValueField || table[i].NormalValueField))
		{
			bool result = false;
			if(strlen(text) == 0)
			{
				RedSystemMessage(m_session, "No values specified.");
			}
			if(table[i].ValueType == 2)
				result = CmdSetFloatField(m_session, table[i].NormalValueField, table[i].MaxValueField, table[i].Name, text);
			else
				result = CmdSetValueField(m_session, table[i].NormalValueField, table[i].MaxValueField, table[i].Name, text);
			if(!result)
				RedSystemMessage(m_session, "Must be in the form of (command) <value>, or, (command) <value> <maxvalue>");
		}
		else
		{
			if(!(this->*(table[i].Handler))(text, m_session))
			{
				if(table[i].Help != "")
					SendMultilineMessage(m_session, table[i].Help.c_str());
				else
				{
					RedSystemMessage(m_session, "Incorrect syntax specified. Try .help %s for the correct syntax.", table[i].Name);
				}
			}
		}

		return true;
	}
	return false;
}

int ChatHandler::ParseCommands(const char* text, WorldSession* session)
{
	if(!session)
		return 0;

	if(!*text)
		return 0;

	if(session->GetPermissionCount() == 0 && sWorld.m_reqGmForCommands)
		return 0;

	if(text[0] != '!' && text[0] != '.') // let's not confuse users
		return 0;

	/* skip '..' :P that pisses me off */
	if(text[1] == '.')
		return 0;

	text++;

	if(!ExecuteCommandInTable(CommandTableStorage::getSingleton().Get(), text, session))
	{
		SystemMessage(session, "There is no such command, or you do not have access to it.");
	}

	return 1;
}

WorldPacket* ChatHandler::FillMessageData(uint32 type, uint32 language, const char* message, uint64 guid , uint8 flag) const
{
	//Packet    structure
	//uint8	    type;
	//uint32	language;
	//uint64	guid;
	//uint64	guid;
	//uint32	len_of_text;
	//char	    text[];		 // not sure ? i think is null terminated .. not null terminated
	//uint8	    afk_state;
	ARCEMU_ASSERT(type != CHAT_MSG_CHANNEL);
	//channels are handled in channel handler and so on
	uint32 messageLength = (uint32)strlen(message) + 1;

	WorldPacket* data = new WorldPacket(SMSG_MESSAGECHAT, messageLength + 30);

	*data << (uint8)type;
	*data << language;

	*data << guid;
	*data << uint32(0);

	*data << guid;

	*data << messageLength;
	*data << message;

	*data << uint8(flag);
	return data;
}

WorldPacket* ChatHandler::FillSystemMessageData(const char* message) const
{
	uint32 messageLength = (uint32)strlen(message) + 1;

	WorldPacket* data = new WorldPacket(SMSG_MESSAGECHAT, 30 + messageLength);
	*data << (uint8)CHAT_MSG_SYSTEM;
	*data << (uint32)LANG_UNIVERSAL;

	// Who cares about guid when there's no nickname displayed heh ?
	*data << (uint64)0;
	*data << (uint32)0;
	*data << (uint64)0;

	*data << messageLength;
	*data << message;

	*data << uint8(0);

	return data;
}

Player* ChatHandler::getSelectedChar(WorldSession* m_session, bool showerror)
{
	uint64 guid;
	Player* chr;

	if(m_session == NULL || m_session->GetPlayer() == NULL) return NULL;

	guid = m_session->GetPlayer()->GetSelection();

	if(guid == 0)
	{
		if(showerror)
			GreenSystemMessage(m_session, "Auto-targeting self.");
		chr = m_session->GetPlayer(); // autoselect
	}
	else
		chr = m_session->GetPlayer()->GetMapMgr()->GetPlayer((uint32)guid);

	if(chr == NULL)
	{
		if(showerror)
			RedSystemMessage(m_session, "This command requires that you select a player.");
		return NULL;
	}

	return chr;
}

Creature* ChatHandler::getSelectedCreature(WorldSession* m_session, bool showerror)
{
	uint64 guid;
	Creature* creature = NULL;

	if(m_session == NULL || m_session->GetPlayer() == NULL) return NULL;

	guid = m_session->GetPlayer()->GetSelection();

	switch( GET_TYPE_FROM_GUID( guid ) ){
		case HIGHGUID_TYPE_PET:
			creature = m_session->GetPlayer()->GetMapMgr()->GetPet(GET_LOWGUID_PART(guid));
			break;

		case HIGHGUID_TYPE_UNIT:
		case HIGHGUID_TYPE_VEHICLE:
			creature = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
			break;
	}

	if(creature != NULL)
		return creature;
	else
	{
		if(showerror)
			RedSystemMessage(m_session, "This command requires that you select a creature.");
		return NULL;
	}
}

void ChatHandler::SystemMessage(WorldSession* m_session, const char* message, ...)
{
	if(!message) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1, 1024, message, ap);
	WorldPacket* data = FillSystemMessageData(msg1);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::ColorSystemMessage(WorldSession* m_session, const char* colorcode, const char* message, ...)
{
	if(!message) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1, 1024, message, ap);
	char msg[1024];
	snprintf(msg, 1024, "%s%s|r", colorcode, msg1);
	WorldPacket* data = FillSystemMessageData(msg);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::RedSystemMessage(WorldSession* m_session, const char* message, ...)
{
	if(!message) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1, 1024, message, ap);
	char msg[1024];
	snprintf(msg, 1024, "%s%s|r", MSG_COLOR_LIGHTRED/*MSG_COLOR_RED*/, msg1);
	WorldPacket* data = FillSystemMessageData(msg);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::GreenSystemMessage(WorldSession* m_session, const char* message, ...)
{
	if(!message) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1, 1024, message, ap);
	char msg[1024];
	snprintf(msg, 1024, "%s%s|r", MSG_COLOR_GREEN, msg1);
	WorldPacket* data = FillSystemMessageData(msg);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::BlueSystemMessage(WorldSession* m_session, const char* message, ...)
{
	if(!message) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1, 1024, message, ap);
	char msg[1024];
	snprintf(msg, 1024, "%s%s|r", MSG_COLOR_LIGHTBLUE, msg1);
	WorldPacket* data = FillSystemMessageData(msg);
	if(m_session != NULL)
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::RedSystemMessageToPlr(Player* plr, const char* message, ...)
{
	if(!message || !plr || !plr->GetSession()) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1, 1024, message, ap);
	RedSystemMessage(plr->GetSession(), (const char*)msg1);
}

void ChatHandler::GreenSystemMessageToPlr(Player* plr, const char* message, ...)
{
	if(!message || !plr || !plr->GetSession()) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1, 1024, message, ap);
	GreenSystemMessage(plr->GetSession(), (const char*)msg1);
}

void ChatHandler::BlueSystemMessageToPlr(Player* plr, const char* message, ...)
{
	if(!message || !plr || !plr->GetSession()) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1, 1024, message, ap);
	BlueSystemMessage(plr->GetSession(), (const char*)msg1);
}

void ChatHandler::SystemMessageToPlr(Player* plr, const char* message, ...)
{
	if(!message || !plr || !plr->GetSession()) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1, 1024, message, ap);
	SystemMessage(plr->GetSession(), msg1);
}

bool ChatHandler::CmdSetValueField(WorldSession* m_session, uint32 field, uint32 fieldmax, const char* fieldname, const char* args)
{
	char* pvalue;
	uint32 mv, av;

	if(!args || !m_session) return false;

	pvalue = strtok((char*)args, " ");
	if(!pvalue)
		return false;
	else
		av = atol(pvalue);

	if(fieldmax)
	{
		char* pvaluemax = strtok(NULL, " ");
		if(!pvaluemax)
			return false;
		else
			mv = atol(pvaluemax);
	}
	else
	{
		mv = 0;
	}

	if(av <= 0 && mv > 0)
	{
		RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
		return true;
	}
	if(fieldmax)
	{
		if(mv < av || mv <= 0)
		{
			RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
			return true;
		}
	}

	Player* plr = getSelectedChar(m_session, false);
	if(plr)
	{
		sGMLog.writefromsession(m_session, "used modify field value: %s, %u on %s", fieldname, av, plr->GetName());
		if(fieldmax)
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %d/%d.", fieldname, plr->GetName(), av, mv);
			GreenSystemMessageToPlr(plr, "%s set your %s to %d/%d.", m_session->GetPlayer()->GetName(), fieldname, av, mv);
		}
		else
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %d.", fieldname, plr->GetName(), av);
			GreenSystemMessageToPlr(plr, "%s set your %s to %d.", m_session->GetPlayer()->GetName(), fieldname, av);
		}

		if(field == UNIT_FIELD_STAT1) av /= 2;
		if(field == UNIT_FIELD_BASE_HEALTH)
		{
			plr->SetHealth(av);
		}

		plr->SetUInt32Value(field, av);

		if(fieldmax)
		{
			plr->SetUInt32Value(fieldmax, mv);
		}
	}
	else
	{
		Creature* cr = getSelectedCreature(m_session, false);
		if(cr)
		{
			if(!(field < UNIT_END && fieldmax < UNIT_END)) return false;
			std::string creaturename = "Unknown Being";
			creaturename = cr->GetCreatureInfo()->Name;
			if(fieldmax)
				BlueSystemMessage(m_session, "Setting %s of %s to %d/%d.", fieldname, creaturename.c_str(), av, mv);
			else
				BlueSystemMessage(m_session, "Setting %s of %s to %d.", fieldname, creaturename.c_str(), av);
			sGMLog.writefromsession(m_session, "used modify field value: [creature]%s, %u on %s", fieldname, av, creaturename.c_str());
			if(field == UNIT_FIELD_STAT1) av /= 2;
			if(field == UNIT_FIELD_BASE_HEALTH)
				cr->SetHealth(av);

			switch(field)
			{
				case UNIT_FIELD_FACTIONTEMPLATE:
					{
						if(cr->m_spawn)
							WorldDatabase.Execute("UPDATE creature_spawns SET faction = %u WHERE entry = %u", av, cr->m_spawn->entry);
					}
					break;
				case UNIT_NPC_FLAGS:
					{
						WorldDatabase.Execute("UPDATE creature_proto SET npcflags = %u WHERE entry = %u", av, cr->GetProto()->Id);
					}
					break;
			}

			cr->SetUInt32Value(field, av);

			if(fieldmax)
			{
				cr->SetUInt32Value(fieldmax, mv);
			}
			// reset faction
			if(field == UNIT_FIELD_FACTIONTEMPLATE)
				cr->_setFaction();

			// Only actually save the change if we are modifying a spawn
			if( cr->GetSQL_id() != 0 )
				cr->SaveToDB();
		}
		else
		{
			RedSystemMessage(m_session, "Invalid Selection.");
		}
	}
	return true;
}

bool ChatHandler::CmdSetFloatField(WorldSession* m_session, uint32 field, uint32 fieldmax, const char* fieldname, const char* args)
{
	char* pvalue;
	float mv, av;

	if(!args || !m_session) return false;

	pvalue = strtok((char*)args, " ");
	if(!pvalue)
		return false;
	else
		av = (float)atof(pvalue);

	if(fieldmax)
	{
		char* pvaluemax = strtok(NULL, " ");
		if(!pvaluemax)
			return false;
		else
			mv = (float)atof(pvaluemax);
	}
	else
	{
		mv = 0;
	}

	if(av <= 0)
	{
		RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
		return true;
	}
	if(fieldmax)
	{
		if(mv < av || mv <= 0)
		{
			RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
			return true;
		}
	}

	Player* plr = getSelectedChar(m_session, false);
	if(plr)
	{
		sGMLog.writefromsession(m_session, "used modify field value: %s, %f on %s", fieldname, av, plr->GetName());
		if(fieldmax)
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %.1f/%.1f.", fieldname, plr->GetName(), av, mv);
			GreenSystemMessageToPlr(plr, "%s set your %s to %.1f/%.1f.", m_session->GetPlayer()->GetName(), fieldname, av, mv);
		}
		else
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %.1f.", fieldname, plr->GetName(), av);
			GreenSystemMessageToPlr(plr, "%s set your %s to %.1f.", m_session->GetPlayer()->GetName(), fieldname, av);
		}
		plr->SetFloatValue(field, av);
		if(fieldmax) plr->SetFloatValue(fieldmax, mv);
	}
	else
	{
		Creature* cr = getSelectedCreature(m_session, false);
		if(cr)
		{
			if(!(field < UNIT_END && fieldmax < UNIT_END)) return false;
			std::string creaturename = "Unknown Being";
			creaturename = cr->GetCreatureInfo()->Name;
			if(fieldmax)
				BlueSystemMessage(m_session, "Setting %s of %s to %.1f/%.1f.", fieldname, creaturename.c_str(), av, mv);
			else
				BlueSystemMessage(m_session, "Setting %s of %s to %.1f.", fieldname, creaturename.c_str(), av);
			cr->SetFloatValue(field, av);
			sGMLog.writefromsession(m_session, "used modify field value: [creature]%s, %f on %s", fieldname, av, creaturename.c_str());
			if(fieldmax)
			{
				cr->SetFloatValue(fieldmax, mv);
			}
			//cr->SaveToDB();
		}
		else
		{
			RedSystemMessage(m_session, "Invalid Selection.");
		}
	}
	return true;
}
