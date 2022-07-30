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

Config ConfigFile::getCurrentSettings() {
	Config c;
	c.fullscreen = globals->fullscreen;
	c.sound = Sound::sound;
	c.music = Sound::music;
	c.skilllevel = SkillLevel::current->level;
	c.highscore = PlayerStats::getHighScore();

	for (int pnum = 0; pnum < 2; pnum++) {
		InputState* is = InputState::playercontrols[pnum];

		int keygroupnum = is->getKeyAxisPair(true);
		c.movegroup[pnum] = InputState::keygroups[keygroupnum];

		keygroupnum = is->getKeyAxisPair(false);
		c.aimgroup[pnum] = InputState::keygroups[keygroupnum];
		
		c.usekey[pnum] = InputState::convert_SDL2(is->usekey);
	}
	return c;
}

Config ConfigFile::getDefaultSettings() {
	Config c;
	// no config file! use default settings
	c.fullscreen = c.music = c.sound = false;
	c.highscore = c.skilllevel = 0;
	c.movegroup[0] = "wasd";
	c.aimgroup[0] = "cursorkeys";
	c.usekey[0] = 63;
	c.movegroup[1] = "ijkl";
	c.aimgroup[1] = "keypad";
	c.usekey[1] = 79;

	return c;
}

ConfigFile::ConfigFile() {
	FILE *f = fopen(getFilename().c_str(), "rb");
	config = getDefaultSettings();
	if (f != NULL)
	{
		IniFile iniFile;
		iniFile.read(f);
		fclose(f);
		config.fullscreen = iniFile.get("FullscreenOn", 0);
		config.music = iniFile.get("MusicOn", 0);
		config.sound = iniFile.get("SoundOn", 0);
		config.highscore = iniFile.get("HSLog", 0);
		config.skilllevel = iniFile.get("SkillLevel", 0);
        
		for (int p=0; p < 2; p++) {
			string base = "Controls/PlayerX";
			base[15] = p + '1'; 
			string mv = iniFile.get(base + "/movement", config.movegroup[p]);
			string am = iniFile.get(base + "/aiming", config.aimgroup[p]);
			config.movegroup[p] = mv;
			config.aimgroup[p] = am;
			config.usekey[p] = iniFile.get(base + "/usekey", config.usekey[p]);
		}
	}
}

void ConfigFile::saveSettings(Config newconf) {
	FILE *f = fopen(getFilename().c_str(), "wb");
	IniFile iniFile;
	iniFile.add("FullscreenOn", (int)newconf.fullscreen);
	iniFile.add("MusicOn", (int)newconf.music);
	iniFile.add("SoundOn", (int)newconf.sound);
	iniFile.add("HSLog", newconf.highscore);
	iniFile.add("SkillLevel", newconf.skilllevel);
	
	for (int p=0; p < 2; p++) {
		string base = "Controls/PlayerX";
		base[15] = p + '1'; 
		iniFile.add(base + "/movement", newconf.movegroup[p]);
		iniFile.add(base + "/aiming", newconf.aimgroup[p]);
		iniFile.add(base + "/usekey", newconf.usekey[p]);
	}
	iniFile.write(f);
	fclose(f);
}

ConfigFile::~ConfigFile() { }
