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
#include "InputState.hh"
#include "Sound.hh"
#include "Globals.hh"
#include "PlayerStats.hh"
#include "SkillLevel.hh"
#include <strstream>

using namespace std;

extern Globals* globals;

string ConfigFile::getFilename() {
#ifdef WIN32
	return "mojotron.ini";
#else
	//#include <stdlib.h>
	return string(getenv("HOME")) + "/.mojotronrc";
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
		
		c.usekey[pnum] = is->usekey;
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
	c.usekey[0] = CL_KEY_SPACE;
	c.movegroup[1] = "ijkl";
	c.aimgroup[1] = "keypad";
	c.usekey[1] = CL_KEY_KP_ENTER;

	return c;
}

ConfigFile::ConfigFile() {
	CL_ResourceManager resman;
	try {
		resman = CL_ResourceManager(getFilename(), false);
	} catch (CL_Error) {
		cout << "Couldn't find configuration file." <<
			" Using default settings" << endl;
		config = getDefaultSettings();
	}
	try {
		config.fullscreen = CL_Integer("FullscreenOn", &resman);
		config.music = CL_Integer("MusicOn", &resman);
		config.sound = CL_Integer("SoundOn", &resman);
		config.highscore = CL_Integer("HSLog", &resman);
		config.skilllevel = CL_Integer("SkillLevel", &resman);
        
		for (int p=0; p < 2; p++) {
			string base = "Controls/PlayerX";
			base[15] = p + '1'; 
			string mv = CL_String(base + "/movement", &resman);
			string am = CL_String(base + "/aiming", &resman);
			config.movegroup[p] = mv;
			config.aimgroup[p] = am;
			config.usekey[p] = CL_Integer(base + "/usekey",&resman);
		}
	} catch (CL_Error err) {
		cout << "Couldn't read configuration file. " <<
			err.message.c_str() <<
			" Using default settings" << endl;
		config = getDefaultSettings();
	}
}

void ConfigFile::saveSettings(Config newconf) {
	ofstream out(getFilename().c_str());
	//assure(out, "Strfile.out");
	
	string tint = " (type=integer);\n";
	string tstr = " (type=string);\n";
	out << "FullscreenOn = " << (int)newconf.fullscreen << tint;
	out << "MusicOn = " << (int)newconf.music << tint;
	out << "SoundOn = " << (int)newconf.sound << tint;
	out << "HSLog = " << newconf.highscore << tint;
	out << "SkillLevel = " << newconf.skilllevel << tint << endl;

	out << "section Controls" << " {" << endl;
	for (int p=0; p < 2; p++) {
		out << "\tsection Player" << (p+1) << " {" << endl;
		out << "\t\tusekey = " << newconf.usekey[p] << tint;
		out << "\t\tmovement = " << newconf.movegroup[p] << tstr;
		out << "\t\taiming = " << newconf.aimgroup[p] << tstr;
		out << "\t}" << endl;
	}
	out << "}" << endl;
}

ConfigFile::~ConfigFile() { }
