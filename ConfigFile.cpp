/*	MOJOTRON, a computer game.
	Copyright (C) 2001-2003 Craig Timpany
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
		      
	This program is distributed in the hope that it will be fun,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
			       
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "ConfigFile.hh"
#include "IniFile.h"
#include "InputState.hh"
#include "Sound.hh"
#include "Globals.hh"
#include "PlayerStats.hh"
#include "SkillLevel.hh"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

extern Globals* globals;

Config Config::config;

Statistics::Statistics() {
	totaltime = 0;
	hits = 0;
	misses = 0;
	fired = 0;
	kills = 0;
	deaths = 0;
	powerups = 0;
	xlives = 0;
	fruit = 0;
	levels = 0;
	warps = 0;
}

void Config::updateCurrentSettings() {
	fullscreen = globals->fullscreen;
	sound = Sound::sound;
	music = Sound::music;
	skilllevel = SkillLevel::current->level;
	highscore = PlayerStats::getHighScore();

	for (int pnum = 0; pnum < 2; pnum++) {
		InputState* is = InputState::playercontrols[pnum];

		int keygroupnum = is->getKeyAxisPair(true);
		movegroup[pnum] = InputState::keygroups[keygroupnum];

		keygroupnum = is->getKeyAxisPair(false);
		aimgroup[pnum] = InputState::keygroups[keygroupnum];

		usekey[pnum] = InputState::convert_SDL2(is->usekey);
	}
}

Config::Config() {
	fullscreen = music = sound = false;
	highscore = skilllevel = 0;
	movegroup[0] = "wasd";
	aimgroup[0] = "cursorkeys";
	usekey[0] = 63;
	movegroup[1] = "ijkl";
	aimgroup[1] = "keypad";
	usekey[1] = 79;
}

string ConfigFile::getFilename() {
#ifdef WIN32
	return "mojotron.ini";
#else
	string result;
	char* env;

	env=getenv("XDG_DATA_HOME");
	if(env!=NULL){
		result = env;
	}else{
		result = getenv("HOME");
		result += "/.local/share";
	}
	result += "/mojotron";
	std::filesystem::create_directories(result);
	return result + "/config";
#endif
}

ConfigFile::ConfigFile() {
	FILE *f = fopen(getFilename().c_str(), "rb");
	if (f != NULL)
	{
		IniFile iniFile;
		iniFile.read(f);
		fclose(f);
		Config::config.fullscreen = iniFile.get("FullscreenOn", 0);
		Config::config.music = iniFile.get("MusicOn", 0);
		Config::config.sound = iniFile.get("SoundOn", 0);
		Config::config.highscore = iniFile.get("HSLog", 0);
		Config::config.skilllevel = iniFile.get("SkillLevel", 0);

		for (int p=0; p < 2; p++) {
			string base = "Controls/PlayerX";
			base[15] = p + '1'; 
			string mv = iniFile.get(base + "/movement", Config::config.movegroup[p]);
			string am = iniFile.get(base + "/aiming", Config::config.aimgroup[p]);
			Config::config.movegroup[p] = mv;
			Config::config.aimgroup[p] = am;
			Config::config.usekey[p] = iniFile.get(base + "/usekey", Config::config.usekey[p]);
		}
		getStatistics(iniFile, "Statistics/Overall", Config::config.overall);
		getStatistics(iniFile, "Statistics/Best", Config::config.best);
	}
}

void ConfigFile::saveSettings() {
	FILE *f = fopen(getFilename().c_str(), "wb");
	IniFile iniFile;
	iniFile.add("FullscreenOn", (int)Config::config.fullscreen);
	iniFile.add("MusicOn", (int)Config::config.music);
	iniFile.add("SoundOn", (int)Config::config.sound);
	iniFile.add("HSLog", Config::config.highscore);
	iniFile.add("SkillLevel", Config::config.skilllevel);

	for (int p=0; p < 2; p++) {
		string base = "Controls/PlayerX";
		base[15] = p + '1'; 
		iniFile.add(base + "/movement", Config::config.movegroup[p]);
		iniFile.add(base + "/aiming", Config::config.aimgroup[p]);
		iniFile.add(base + "/usekey", Config::config.usekey[p]);
	}
	saveStatistics(iniFile, "Statistics/Overall", Config::config.overall);
	saveStatistics(iniFile, "Statistics/Best", Config::config.best);
	iniFile.write(f);
	fclose(f);
}

ConfigFile::~ConfigFile() { }

void ConfigFile::getStatistics(IniFile &iniFile, const std::string &prefix, Statistics &stats) {
	stats.totaltime = iniFile.get(prefix + "/totaltime", 0);
	stats.hits = iniFile.get(prefix + "/hits", 0);
	stats.misses = iniFile.get(prefix + "/misses", 0);
	stats.fired = iniFile.get(prefix + "/fired", 0);
	stats.kills = iniFile.get(prefix + "/kills", 0);
	stats.deaths = iniFile.get(prefix + "/deaths", 0);
	stats.powerups = iniFile.get(prefix + "/powerups", 0);
	stats.xlives = iniFile.get(prefix + "/xlives", 0);
	stats.fruit = iniFile.get(prefix + "/fruit", 0);
	stats.levels = iniFile.get(prefix + "/levels", 0);
	stats.warps = iniFile.get(prefix + "/warps", 0);
}

void ConfigFile::saveStatistics(IniFile &iniFile, const std::string &prefix, Statistics &stats) {
	iniFile.add(prefix + "/totaltime", stats.totaltime);
	iniFile.add(prefix + "/hits", stats.hits);
	iniFile.add(prefix + "/misses", stats.misses);
	iniFile.add(prefix + "/fired", stats.fired);
	iniFile.add(prefix + "/kills", stats.kills);
	iniFile.add(prefix + "/deaths", stats.deaths);
	iniFile.add(prefix + "/powerups", stats.powerups);
	iniFile.add(prefix + "/xlives", stats.xlives);
	iniFile.add(prefix + "/fruit", stats.fruit);
	iniFile.add(prefix + "/levels", stats.levels);
	iniFile.add(prefix + "/warps", stats.warps);
}
