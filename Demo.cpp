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

#include <SDL.h>
#include <iostream>
#include "Thing.hh"
#include "Sprite.hh"
#include "Demo.hh"
#include "ObjectList.hh"
#include "Player.hh"
#include "InputState.hh"
#include "Display.hh"
#include "Intro.hh"

#define MAXLINEWIDTH 80
#define MINUSDIR 0
#define ZERODIR 1
#define PLUSDIR 2

using namespace std;
extern World* worldobj;
extern Globals* globals;

Demo::Demo() {
	record = true;
	maxdemotime = -1;
	sur_key = globals->loadSurface("Surfaces/key");
	sur_spacekey = globals->loadSurface("Surfaces/spacekey");
	sur_key_select = globals->loadSurface("Surfaces/keyselected");
	sur_spacekey_select = globals->loadSurface("Surfaces/spacekeyselected");
}

Demo::Demo(std::string filename) {
	record = false;
	maxdemotime = -1;
	snaptime = 0;
	starttime = 0;
	r = g = b = 0;
	for (int ii=0; ii < MAX_OBJS; ii++) {
		// to cause crash if an unfilled index is used
		spr[ii] = -1;
	}
	obj = -1;

	sur_key = globals->loadSurface("Surfaces/key");
	sur_spacekey = globals->loadSurface("Surfaces/spacekey");
	sur_key_select = globals->loadSurface("Surfaces/keyselected");
	sur_spacekey_select = globals->loadSurface("Surfaces/spacekeyselected");

	FILE *f = fopen(filename.c_str(), "r");
	if (f)	{ if (globals->verbosity > 0)	cout << "Loading demo " << filename << endl; }
	else	cout << filename << " not found." << endl;

	char buffer[MAXLINEWIDTH];
	std::string str;
	do {
		if (!fgets(buffer, MAXLINEWIDTH, f)) break;
		str = std::string(buffer);
	} while (readPosition(str));
	fclose(f);
}

Demo::~Demo() {
}

void Demo::writeSnapshot(int time) {
	snaptime = time;

	// redirect me. :)
	cout << "Time:" << snaptime << " ";
	cout << worldobj->serialiseBG();
	cout << worldobj->playerlist[0]->in->serialiseKeys();
	cout << endl;

	worldobj->serialiseAll();
}

bool Demo::readPosition(std::string record) {
	int x, y;
	int status = sscanf(record.c_str(), "%d,%d\n", &x, &y);
	if (status == 2) {
		startpos.push_back(std::make_pair(x, y));
	}

	return (status == 2);
}

void Demo::drawThing() {
	globals->spr[(spr[obj])]->draw(
		(x[obj])<<8, (y[obj])<<8, 
		frame[obj], (bool)(faceleft[obj]));
}

void Demo::drawWorld() {
	SDL_SetRenderDrawColor(game_renderer, 0, 0, 0, 255);
	SDL_RenderClear(game_renderer);
	SDL_Rect rec;
	SDL_SetRenderDrawColor(game_renderer, ((float)r/100) * 255, ((float)g/100) * 255, ((float)b/100) * 255, 255);
	rec.x = 0;
	rec.y = 0;
	rec.w = XWINSIZE>>8;
	rec.h = (YWINSIZE>>8)+PAD;
	SDL_RenderFillRect(game_renderer, &rec);
}

void Demo::drawAxisPair(int player, bool moveaxispair,
			int xaxis, int yaxis, int xpos, int ypos) {

	std::string name;
	if (moveaxispair)	name = "Movement Controls";
	else			name = "Aiming Controls";
	globals->mediumfont->print_center(xpos+72, ypos-40, name.c_str());

	int axis, lowkeyx, lowkeyy, highkeyx, highkeyy;

	for (int axisnum = 0; axisnum < 2; axisnum++) {
		switch (axisnum) {
			case 0:
				axis = xaxis;
				lowkeyx = xpos;			lowkeyy = ypos + KEYHEIGHT;
				highkeyx = xpos + 2*KEYWIDTH;	highkeyy = ypos + KEYHEIGHT;
				break;
			case 1:
				axis = yaxis;
				lowkeyx = xpos + KEYWIDTH;	lowkeyy = ypos;
				highkeyx = xpos + KEYWIDTH;	highkeyy = ypos + KEYHEIGHT;
				break;
			default:
//				throw CL_Error("Stark raving insanity in Demo::drawAxisPair");
				break;
		}

		// draw key for low direction of axis
		if (axis == 0)	sur_key_select->put_screen(lowkeyx, lowkeyy);
		else		sur_key->put_screen(lowkeyx, lowkeyy);

		char lowchar[] = "z";
		lowchar[0] = InputState::playercontrols[player]->getKeyChar(moveaxispair, axisnum, -1);
		globals->smallfont->print_center(lowkeyx+XKEYCENT, lowkeyy+YKEYCENT, lowchar);

		// draw key for high direction
		if (axis == 2)	sur_key_select->put_screen(highkeyx, highkeyy);
		else		sur_key->put_screen(highkeyx, highkeyy);

		char highchar[] = "z";
		highchar[0] = InputState::playercontrols[player]->getKeyChar(moveaxispair, axisnum, 1);
		globals->smallfont->print_center(highkeyx+XKEYCENT, highkeyy+YKEYCENT, highchar);
	}
}

void Demo::drawBonus(int player, int xpos, int ypos) {
	int skeyxorigin = xpos;
	int skeyyorigin = ypos;

	globals->mediumfont->print_left(skeyxorigin, skeyyorigin, "Activate Powerup");
	sur_spacekey->put_screen(skeyxorigin, skeyyorigin+40);
	globals->smallfont->print_left(skeyxorigin+XKEYCENT, skeyyorigin+40+YKEYCENT, 
					InputState::playercontrols[player]->getUseKey().c_str());
}

void Demo::updateDemotime(int demotime)
{
	if (maxdemotime == -1 || maxdemotime < demotime) {
		maxdemotime = demotime;
		startpos = currentpos;
	}
	currentpos.clear();
}
