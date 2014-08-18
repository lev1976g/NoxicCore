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

#ifndef __WEATHERMGR_H
#define __WEATHERMGR_H

#define WEATHER_DENSITY_UPDATE 0.05f
#define WEATHER_SEASONS 4
struct WeatherSeasonChances
{
    uint32 rainChance;
    uint32 snowChance;
    uint32 stormChance;
};

struct WeatherData
{
    WeatherSeasonChances data[WEATHER_SEASONS];
};

/// Weather defines
enum WeatherTypes
{
    WEATHER_TYPE_NORMAL				= 0, // NORMAL
    WEATHER_TYPE_FOG				= 1, // FOG --> current value irrelevant
    WEATHER_TYPE_RAIN				= 2, // RAIN
    WEATHER_TYPE_LIGHT_RAIN			= 3,
    WEATHER_TYPE_MEDIUM_RAIN		= 4,
    WEATHER_TYPE_HEAVY_RAIN			= 5,
    WEATHER_TYPE_SNOW				= 8, // SNOW
    WEATHER_TYPE_LIGHT_SNOW			= 6,
    WEATHER_TYPE_MEDIUM_SNOW		= 7,
    WEATHER_TYPE_HEAVY_SNOW			= 8,
    WEATHER_TYPE_SANDSTORM			= 16 // SANDSTORM
    WEATHER_TYPE_LIGHT_SANDSTORM	= 22,
    WEATHER_TYPE_MEDIUM_SANDSTORM	= 41,
    WEATHER_TYPE_HEAVY_SANDSTORM	= 42,
    WEATHER_TYPE_THUNDERS			= 86,
    WEATHER_TYPE_BLACKRAIN			= 90,
	WEATHER_TYPE_BLACKSNOW			= 106
};

enum WeatherSounds
{
    WEATHER_NOSOUND                = 0,
    WEATHER_RAINLIGHT              = 8533,
    WEATHER_RAINMEDIUM             = 8534,
    WEATHER_RAINHEAVY              = 8535,
    WEATHER_SNOWLIGHT              = 8536,
    WEATHER_SNOWMEDIUM             = 8537,
    WEATHER_SNOWHEAVY              = 8538,
    WEATHER_SANDSTORMLIGHT         = 8556,
    WEATHER_SANDSTORMMEDIUM        = 8557,
    WEATHER_SANDSTORMHEAVY         = 8558
};

class WeatherInfo;
class WeatherMgr;

void BuildWeatherPacket(WorldPacket* data, uint32 Effect, float Density);
uint32 GetSound(uint32 Effect, float Density);

class WeatherMgr :  public Singleton <WeatherMgr>
{
	public:
		WeatherMgr();
		~WeatherMgr();

		void LoadFromDB();
		void SendWeather(Player* plr);

	private:
		std::map<uint32, WeatherInfo*> m_zoneWeathers;
};

class WeatherInfo : public EventableObject
{
	friend class WeatherMgr;

	public:
		WeatherInfo();
		~WeatherInfo();

		void BuildUp();
		void Update();
		void SendUpdate();
		void SendUpdate(Player* plr);

	protected:
		void _GenerateWeather();

		uint32 m_zoneId;

		uint32 m_totalTime;
		uint32 m_currentTime;

		float m_maxDensity;
		float m_currentDensity;

		uint32 m_currentEffect;
		std::map<uint32, uint32> m_effectValues;
};

#define sWeatherMgr WeatherMgr::getSingleton()

#endif
