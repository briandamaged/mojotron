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

#ifndef WORLD_HH
#define WORLD_HH

#include "ScoreBoard.hh"
#include "Display.hh"

#define SCRFILE "res.scr"
#define MAX_OBJS 255
#define MAX_PLAYERS 2

class Thing;
class Player;
class Sprite;
class PlayerStats;
class MiscDisplay;
class ObjectList;
class Demo;
class Menu;
	
class World {
public:
	enum GameState { PLAYING, DIED, PAUSED, 
			WON, WARPING, WARPED, HELPING, NEXTLEV, 
			GAMEOVER, EXIT, DELAY, READY, ENDGAME };
	World::GameState state; 
	World::GameState oldstate; // what state gets switched to after delay times out, or game is unpaused
	
	Sprite *spr_grunt;
	Player* playerlist[MAX_OBJS];	// not Thing*

	void load_level(int lev);
	Thing* getNearestPlayer(int xpos, int ypos);
	void addThing(Thing* newthing);
	void removeThing(Thing* deadthing);
	void removePlayer(int number);
	void setConsole(std::string newstr, int duration);
	void gameDelay(int duration, 	World::GameState statebefore, 
					World::GameState stateafter);
	int getLevelAge();
	int getWarpRemaining() { return warp_time - levelage/1000; }
	float getBGRed();
	float getBGGreen();
	float getBGBlue();
	std::string serialiseBG();
	void serialiseAll();

	PlayerStats* getStats(int player);
	World(bool demo = false);
	~World();
	int run(bool recorddemo); // here lies the game loop
	void playing();
	void highlightFruit();

	MiscDisplay* miscdisplay;
	int flashing;

	int num_active, num_lev, max_active;
	int num_players;

	int warp_time, delta;
	int levelage, timesinceready;
	int deltamodify;
	float monster_speed, startaccel;
	float orig_monster_speed;	// before num_lev modification
	float monster_quantity;
	int monster_size;

	int lives;

	void updateDisplay();
	
private: 
	bool readyforkey;
	int inittime, pausetimer;
	unsigned long cltime, oldtime;
	int statetime, delay;
	int startaccellength;
	int maxdelta;
	int mindelta;

	int bgred, bggreen, bgblue;
	int bgtile[15][10];
	CL_Surface tiles;
	CL_Surface ui_bg;
	std::string lev_name;

	Menu* pausemenu;
	
	void cleanUp();
	void clearAll();
	void makeLists();
	void findPlayerCols();
	void findEnemyCols();
	void findObstacleCols();
	void drawBG(int delta);
	void drawScoreBG();
	void listenPauseToggle();
	bool listenAnyKey();
	void maintainDelay();
	//void newPlayerCheck();
	//void addInactivePlayer();
	//void activatePlayer();
	void makePlayer(int i, bool demo);
	void restart();

	int loadlevInt(std::string dataname);
	std::string loadlevString(std::string dataname);
	std::string base;
	
	ObjectList* all;
	ObjectList* elist;
	ObjectList* ellist;
	ObjectList* olist;
	ObjectList* ollist;
	ObjectList* pllist;
	int pindex;
};

#endif
