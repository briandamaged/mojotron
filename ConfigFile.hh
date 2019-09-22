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

#ifndef CONFIGFILE_HH
#define CONFIGFILE_HH

#include <string>

struct Config {
	bool fullscreen, sound, music;
	int skilllevel, highscore;
	std::string movegroup[2];	// one for each player
	std::string aimgroup[2];
	int usekey[2];
};

class ConfigFile {
public:
	ConfigFile();
	void saveSettings(Config newconf);
	~ConfigFile();

	Config getDefaultSettings();
	Config getFileSettings() { return config; }
	// requires globals and inputstate to have been initialised
	Config getCurrentSettings(); 

private:
	std::string getFilename();
	Config config;
};

#endif
