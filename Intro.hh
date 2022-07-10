/*	MOJOTRON, a computer game.
	Copyright (C) 2001 Craig Timpany
	
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

#ifndef INTRO_HH
#define INTRO_HH

#include "SDL_CL_Surface.h"
#include <string>

#define XKEYCENT 20
#define YKEYCENT 12
#define KEYWIDTH 48
#define KEYHEIGHT 48
#define STORYTIME storytime
#define CONTROLSSTART (STORYTIME+9000)
#define POWERUPSTART (CONTROLSSTART+9000)
#define FRUITSTART (POWERUPSTART+9000)
#define ENEMYSTART (FRUITSTART+9000)
#define RESTART (ENEMYSTART+0)

class Demo;

class Intro {
public:
	static bool show();
	static void deinit();
	static void drawBg();
	static void demoBg();

	static CL_Surface* sur_skillicons;
	static Demo* demo;
private:
	static void story();
	static void powerups();
	static void fruitTable();
	static void enemies();
	static void drawEnemyLine(	int line, int s, 
					std::string name, std::string info);
	static void controlsDemo();
	static void restart(bool active);

	static int timer;
	static int delta;
	static int prevtime;
	static int demotime;
	static int restarttime;
	static int introstarttime;
	static int slomoval;

	static int storytime;

	static std::string subtitle;

	static CL_Surface* sur_bubble;
	static CL_Surface* sur_starfield;
	static CL_Surface* sur_title;
	static CL_Surface* sur_reel;
	static int fruit[9];
	static std::vector<std::string> text;
	static int textstart;
	static int textincrement;
};

#endif
