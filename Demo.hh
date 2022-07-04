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

#ifndef DEMO_HH
#define DEMO_HH

#include "SDL_CL_Surface.h"
#include "World.hh"

class Demo {
private:
	FILE* f;
	// current snapshot details
	int r, g, b;
	int x[MAX_OBJS], y[MAX_OBJS], spr[MAX_OBJS];
	int frame[MAX_OBJS], faceleft[MAX_OBJS];
	int obj;	// current object to draw

	int starttime, snaptime;

	void drawThing();	// draws obj
	bool readThing(std::string record);	// reads and draws

	bool readSnapshot(char* line);

	CL_Surface* sur_key;
	CL_Surface* sur_spacekey;
	CL_Surface* sur_key_select;
	CL_Surface* sur_spacekey_select;

public:
	Demo();	// for recording to stdout
	Demo(std::string filename);	// for playback
	~Demo();

	bool play(int time);
	void restart(int timeoffset);
	void test();
	void writeSnapshot(int time);

	void drawWorld();
	void drawAxisPair(int player, bool moveaxispair, 
			int xaxis, int yaxis, int xpos, int ypos);
	void drawBonus(int player, int xpos=100, int ypos=300);

	int xmov, ymov, xaim, yaim, use;
};

#endif
