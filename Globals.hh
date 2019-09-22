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

#ifndef GLOBALS_HH
#define GLOABLS_HH

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
using namespace std;

#define MAX_LOADSTATE 4
#define GLOBALSRESFILE "res.scr"
/*	To add a new sprite requires:
	Adding the resource path to resname
	Adding it to the index enum in Globals.hh
	mention in res.scr
*/

class Sprite;
class Sound;
class SkillLevel;

class Globals {
public:
	enum sprindex {
		PLAYERONE = 0,
		PLAYERTWO,
		PLAYERTHREE,
		PLAYERFOUR,

		SINGLEGRUNT,
		BULLET, 
		PELLET, 
		TRASH,
		CRATE,
		FORCEFIELD,
		BALL,
		OBSTACLE,
		ASTEROID,
		SNIPER,
		FASTSNIPER,
		BOMB,
		SPIKER,
		RIGHTANGLER,
		SIMPLEBOSS,
		CIRCLEBOSS,
		SPIKE,
		POD,
		SIGNAL,
		FLAME,
		SPARK,
		DROID,
		ROCKMAN,
		TURRET,
		ROUNDBULLET,
		SOLDIER,
		DRONE,
		CENTISEG,
		CENTIHEAD,
		CELLPHONE,
		SPIRAL,
		LASERHORIZ,
		LASERVERT,
		GOREEXPLODE,
		METALLICEXPLODE,
		FLAMEEXPLODE,
		BULLETSOAKED,
		REEL,
		BONUSBUBBLE,
		BONUSXLIFE,
		BONUSTELEPORT,
		BONUSSHADES,
		BONUSTHREEWAY,
		BONUSREARFIRE,
		BONUSFLAME,
		BONUSSHIELD,
		BONUSREFLECTION,
		BONUSMINES,
		BONUSBOMB,
		BONUSSPEEDUP,
		BONUSSHRINK,
		BONUSFRUIT,
		BONUSXTIME,
		BONUSREPEL,
		BONUSFREEZE,
		BONUSSLOW,
		BONUSDROID,
		BONUSMACHINEGUN,
		BONUSCRATE,
		SPREND
	};

	bool fullscreen;
	bool vertdisplay;
	bool showloadscreens;
	int verbosity;
	int num_players;
	CL_InputBuffer* buffer;
	CL_ResourceManager* manager;
	
	// game-wide surfaces
	Sprite* spr[SPREND];

	// misc graphics
		CL_Font* smallfont;
		CL_Font* largefont;
		CL_Font* mediumfont;
		CL_Surface* sur_testing;

	Globals(string resfile = GLOBALSRESFILE);
	~Globals();

	void loadSprites();
	void loadingScreen(int loadstarttime);
	void fadeScreen(int timefromtransition, int length);	// run me before drawing things
	void unfade();
	void debugPrint(std::string message, int threshold);
	
	int loadInt(std::string dataname);
	std::string loadString(std::string dataname);
	CL_Surface* loadSurface(std::string dataname);
	void cycleSkill();

	private:
	int loadstate;
	std::map<std::string, int> loadedints;
};

#endif

