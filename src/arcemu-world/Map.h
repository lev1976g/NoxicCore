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

#ifndef __MAP_H
#define __MAP_H

enum MapNames
{
	/// World of Warcraft
	// Eastern Kingdoms
	MAP_BLASTED_LANDS						= 4,
	MAP_SWAMP_OF_SORROWS					= 8,
	MAP_BLACKROCK_MOUNTAIN					= 25,
	MAP_DEADWIND_PASS						= 41,
	MAP_BURNING_STEPPES						= 46,
	MAP_SEARING_GORGE						= 51,
	MAP_BADLANDS							= 3,
	MAP_EASTERN_PLAGUELANDS					= 139,
	MAP_WESTERN_PLAGUELANDS					= 28,
	MAP_STRANGLETHORN_VALE					= 33,
	MAP_THE_HINTERLANDS						= 47,
	MAP_ARATHI_HIGHLANDS					= 45,
	MAP_DUSKWOOD							= 10,
	MAP_WETLANDS							= 11,
	MAP_HILLSBRAD_FOOTHILLS					= 267,
	MAP_REDRIDGE_MOUNTAINS					= 44,
	MAP_LOCH_MODAN							= 38,
	MAP_SILVERPINE_FOREST					= 130,
	MAP_WESTFALL							= 40,
	MAP_ELWYNN_FOREST						= 12,
	MAP_DUN_MOROGH							= 1,
	MAP_TIRISFAL_GLADES						= 85,
	MAP_DEEPRUN_TRAM						= 2257,
	BATTLEGROUND_ALTERAC_VALLEY				= 2597,
	BATTLEGROUND_ARATHI_BASIN				= 3358,
	// Kalimdor
	MAP_SILITHUS							= 1377,
	MAP_UNGORO_CRATER						= 490,
	MAP_WINTERSPRING						= 618,
	MAP_FELWOOD								= 361,
	MAP_TANARIS								= 440,
	MAP_THOUSAND_NEEDLES					= 400,
	MAP_DUSTWALLOW_MARSH					= 15,
	MAP_FERALAS								= 357,
	MAP_DESOLACE							= 405,
	MAP_STONETALON_MOUNTAINS				= 406,
	MAP_ASHENVALE							= 331,
	MAP_MOONGLADE							= 493,
	MAP_DARKSHORE							= 148,
	MAP_AZSHARA								= 16,
	MAP_BARRENS								= 17,
	MAP_TELDRASSIL							= 141,
	MAP_DUROTAR								= 14,
	MAP_MULGORE								= 215,
	MAP_GM_ISLAND							= 876,
	MAP_THE_VEILED_SEA						= 457,
	/// The Burning Crusade
	// Eastern Kingdoms
	MAP_EVERSONG_WOODS				= 3430,
	MAP_GHOSTLANDS					= 3433,
	// Kalimdor
	MAP_BLOODMYST_ISLE				= 3525,
	MAP_AZUREMYST_ISLE				= 3524,
	MAP_ISLE_OF_QUELDANAS			= 4080,
	// Outland
	MAP_SHADOWMOON_VALLEY			= 3520,
	MAP_NETHERSTORM					= 3523,
	MAP_BLADES_EDGE_MOUNTAINS		= 3522,
	MAP_NAGRAND						= 3518,
	MAP_TEROKKAR_FOREST				= 3519,
	MAP_ZANGARMARSH					= 3521,
	MAP_HELLFIRE_PENINSULA			= 3483,
	BATTLEGROUND_EYE_OF_THE_STORM	= 3820,
	/// Wrath of the Lich King
	// Eastern Kingdoms
	MAP_PLAGUELANDS_THE_SCARLET_ENCLAVE	= 4298,
	// Northrend
	MAP_CRYSTALSONG_FOREST				= 2817,
	MAP_WINTERGRASP						= 4197,
	MAP_HROTHGARS_LANDING				= 4742,
	MAP_ICECROWN						= 210,
	MAP_THE_STORM_PEAKS					= 67,
	MAP_SHOLAZAR_BASIN					= 3711,
	MAP_ZULDRAK							= 66,
	MAP_GRIZZLY_HILLS					= 394,
	MAP_DRAGON_BLIGHT					= 65,
	MAP_BOREAN_TUNDRA					= 3537,
	MAP_HOWLING_FJORD					= 495,
	BATTLEGROUND_ISLE_OF_CONQUEST		= 4710,
	BATTLEGROUND_STRAND_OF_THE_ANCIENTS	= 4384,
	/// Cataclysm
	// Eastern Kingdoms
	//MAP_RUINS_OF_GILNEAS					= 4706,
	//MAP_GILNEAS								= 4714,
	//MAP_GILNEAS_CITY						= 4755,
	//BATTLEGROUND_THE_BATTLE_OF_GILNEAS		= 5449,
	//BATTLEGROUND_TWIN_PEAKS					= 5031,
	// Kalimdor
	//MAP_MOLTEN_FRONT						= 5733,
	//MAP_ULDUM								= 5034,
	//MAP_MOUNT_HYJAL						= 616,
	//MAP_AHNQIRAJ_THE_FALLEN_KINGDOM			= 5695,
	//MAP_NORTHERN_BARRENS					= 17,
	//MAP_SOUTHERN_BARRENS					= 4709,
	//MAP_TWILIGHT_HIGHALANDS					= 4922,
	//MAP_VASHJIR_ABYSSAL_DEPTHS				= 5145,
	//MAP_VASHJIR_SHIMMERING_EXPANSE			= 5144,
	//MAP_VASHJIR								= 4146,
	//MAP_THE_STRANGLETHORN_VALE				= 5339,
	//MAP_THE_CAPE_OF_STRANGLETHORN			= 5287,
	//MAP_NORTHERN_STRANGLETHORN				= 33,
	// The Maelstrom
	//MAP_TOL_BARAD_PENINSULA					= 5389,
	//MAP_TOL_BARAD							= 5095,
	//MAP_DEEPHOLM							= 5042,
	//MAP_DARKMOON_ISLAND						= 5861,
	//MAP_THE_LOST_ISLES						= 4720,
	//MAP_KEZAN								= 4737,
	//MAP_THE_MAELSTROM						= 5416,
	// Kalimdor
	//MAP_AMMEN_VALE						= 6456,
	//MAP_SHADOWGLEN						= 6450,
	//MAP_ECHO_ISLES						= 6453,
	//MAP_CAMP_NARACHE					= 6452,
	//MAP_VALLEY_OF_TRIALS				= 6451,
	//MAP_COLDRIDGE_VALLEY				= 6176,
	//MAP_NORTHSHIRE						= 6170,
	//MAP_NEW_TINKERTOWN					= 6457,
	//MAP_DEATHKNELL						= 6454,
	//MAP_SUNSTRIDER_ISLE					= 6455,
	// The Maelstrom
	//MAP_THE_WANDERING_ISLE				= 5736,
	// Pandaria
	//MAP_VALE_OF_ETERNAL_BLOSSOMS		= 5840,
	//MAP_ISLE_OF_GIANTS					= 6661,
	//MAP_THE_SITUATION_IN_DALARAN		= 6611,
	//MAP_ISLE_OF_THUNDER					= 6507,
	//MAP_TIMELESS_ISLE					= 6757,
	//MAP_DREAD_WASTES					= 6138,
	//MAP_TOWNLONG_STEPPES				= 5842,
	//MAP_THE_VEILED_STAIR				= 6006,
	//MAP_KRASARANG_WILDS					= 6134,
	//MAP_KUN_LAI_SUMMIT					= 5841,
	//MAP_VALLEY_OF_THE_FOUR_WINDS		= 5805,
	//MAP_THE_JADE_FOREST					= 5785,
	//BATTLEGROUND_DEEPWIND_GORGE			= 6665,
	//BATTLEGROUND_TEMPLE_OF_KOTMOGU		= 6051,
	//BATTLEGROUND_SILVERSHARD_MINES		= 6126,
	/// Warlords of Draenor
	// Eastern Kingdoms
	//MAP_BLASTED_LANDS_DO_NOT_USE		= 6936,
	//BATTLEGROUND_HILLSBRAD_FOOTHILLS	= 7107,
	// Draenor
	//MAP_FARALOHN						= 6756,
	//MAP_NAGRAND_DRAENOR					= 6755,
	//MAP_TALADOR							= 6662,
	//MAP_GORGROND						= 6721,
	//MAP_SPIRES_OF_ARAK					= 6722,
	//MAP_FROSTFIRE_RIDGE					= 6720,
	//MAP_SHADOWMOON_VALLEY_DRAENOR		= 6719,
	//MAP_DEFENSE_OF_KARABOR				= 7083,
	//MAP_FROSTWALL						= 7004,
	//MAP_GREAT_HALL						= 7230,
	//MAP_TOWN_HALL						= 7229,
	//MAP_TANAAN_JUNGLE					= 6723,
	//MAP_LUNARFALL						= 7078,
	//MAP_SHATTRATH_CITY					= 6980,
	//MAP_HEROES_THROUGH_TIME				= 6908,
	//MAP_ASHRAN_TRANSITION_ZONE			= 6975,
	//MAP_ASHRAN							= 6941,
	//MAP_DEFENSE_OF_KARABOR			= 7083,
};

class MapMgr;
struct MapInfo;
class TerrainMgr;

#include "TerrainMgr.h"

struct Formation;

typedef struct
{
	uint32	id;//spawn ID
	uint32	entry;
	float	x;
	float	y;
	float	z;
	float	o;
	Formation* form;
	uint8 movetype;
	uint32 displayid;
	uint32 factionid;
	uint32 flags;
	uint32 bytes0;
	uint32 bytes1;
	uint32 bytes2;
	uint32 emote_state;
	//uint32 respawnNpcLink;
	uint16 channel_spell;
	uint32 channel_target_go;
	uint32 channel_target_creature;
	uint16 stand_state;
	uint32 death_state;
	uint32 MountedDisplayID;
	uint32 Item1SlotDisplay;
	uint32 Item2SlotDisplay;
	uint32 Item3SlotDisplay;
	uint32 CanFly;
	uint32 phase;

	/* sets one of the bytes of an uint32 */
	uint32 setbyte(uint32 buffer, uint8 index, uint32 byte)
	{

		/* We don't want a segfault, now do we? */
		if(index >= 4)
			return buffer;

		byte = byte << index * 8;
		buffer = buffer | byte;

		return buffer;
	}
} CreatureSpawn;

typedef struct
{
	uint32	id;//spawn ID
	uint32	entry;
	float	x;
	float	y;
	float	z;
	float	o;
	float	o1;
	float	o2;
	float	o3;
	float	facing;
	uint32	flags;
	uint32	state;
	uint32	faction;
	//uint32 level;
	float scale;
	//uint32 stateNpcLink;
	uint32 phase;
	uint32 overrides;
} GOSpawn;

typedef std::vector<CreatureSpawn*> CreatureSpawnList;
typedef std::vector<GOSpawn*> GOSpawnList;

typedef struct
{
	CreatureSpawnList CreatureSpawns;
	GOSpawnList GOSpawns;
} CellSpawns;

class SERVER_DECL Map
{
	public:
		Map(uint32 mapid, MapInfo* inf);
		~Map();

		ARCEMU_INLINE string GetNameString() { return name; }
		ARCEMU_INLINE const char* GetName() { return name.c_str(); }
		ARCEMU_INLINE MapEntry* GetDBCEntry() { return me; }

		ARCEMU_INLINE CellSpawns* GetSpawnsList(uint32 cellx, uint32 celly)
		{
			ARCEMU_ASSERT(cellx < _sizeX);
			ARCEMU_ASSERT(celly < _sizeY);
			if(spawns[cellx] == NULL) return NULL;

			return spawns[cellx][celly];
		}
		ARCEMU_INLINE CellSpawns* GetSpawnsListAndCreate(uint32 cellx, uint32 celly)
		{
			ARCEMU_ASSERT(cellx < _sizeX);
			ARCEMU_ASSERT(celly < _sizeY);
			if(spawns[cellx] == NULL)
			{
				spawns[cellx] = new CellSpawns*[_sizeY];
				memset(spawns[cellx], 0, sizeof(CellSpawns*)*_sizeY);
			}

			if(spawns[cellx][celly] == 0)
				spawns[cellx][celly] = new CellSpawns;
			return spawns[cellx][celly];
		}

		void LoadSpawns(bool reload);//set to true to make clean up
		uint32 CreatureSpawnCount;
		uint32 GameObjectSpawnCount;

		ARCEMU_INLINE void CellGoneActive(uint32 x, uint32 y)
		{
		}

		ARCEMU_INLINE void CellGoneIdle(uint32 x, uint32 y)
		{
		}

	private:
		MapInfo* 	   _mapInfo;
		uint32 _mapId;
		string name;
		MapEntry* me;

		//new stuff
		CellSpawns** spawns[_sizeX];
	public:
		CellSpawns staticSpawns;
};

#endif
