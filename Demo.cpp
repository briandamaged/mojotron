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

//#include <ClanLib/core.h>
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
	f = NULL;
	sur_key = globals->loadSurface("Surfaces/key");
	sur_spacekey = globals->loadSurface("Surfaces/spacekey");
	sur_key_select = globals->loadSurface("Surfaces/keyselected");
	sur_spacekey_select = globals->loadSurface("Surfaces/spacekeyselected");
}

Demo::Demo(std::string filename) {
	snaptime = 0;
	starttime = 0;
	xmov = ymov = xaim = yaim = use = 0;
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

	f = fopen(filename.c_str(), "r");
	if (f)	{ if (globals->verbosity > 0)	cout << "Loading demo " << filename << endl; }
	else	cout << filename << " not found." << endl;
}

Demo::~Demo() {
	if (f)	fclose(f);
	f = NULL;
}

void Demo::restart(int timeoffset) {
	rewind(f);
	obj = -1;
	snaptime = 0;
	starttime = timeoffset;
}

void Demo::test() {
	bool ismore = true;
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	while (!(state[SDL_SCANCODE_ESCAPE] || !ismore)) {
		ismore = play(SDL_GetTicks());
		InputState::process();
		SDL_RenderPresent(game_renderer);
	}
}

bool Demo::play(int time) {
	time -= starttime;

	bool readnewshot = false;
	char buffer[MAXLINEWIDTH];

	if (!f) return false;

	// what happens when time progresses faster than we can search?
	// this may play slower on slow machines, but not faster on fast ones
	while (snaptime < time) {

		// advance to next snapshot
		if (!fgets(buffer, MAXLINEWIDTH, f)) {
			// couldn't get line
			return false;
		}
		/* TODO: have some way of resetting the time during playback
		if (buffer[0] == 'r') {
			cout << "resetting" << endl;
			starttime = time;
			time = 0;
		}*/
		readSnapshot(buffer);
		readnewshot = true;
	};
	// leaves once it has a snapshot from just into the future

	drawWorld();

	// draw the objects
	if (readnewshot) {
		// new frame from disk
		std::string str;
		obj = -1;
		do {
			if (!fgets(buffer, MAXLINEWIDTH, f)) return false;
			str = std::string(buffer);
		} while (readThing(str));
		obj--;	// last loop returned false, but incremented anyway
	} else {
		SDL_Delay(snaptime - time);
		// no frame read yet
		if (obj == -1)	return true;

		// redraw last objects
		int max = obj;
		for (obj = 0; obj <= max; obj++) {
			drawThing();
		}
		obj--;
	}

	return true;
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

bool Demo::readSnapshot(char* line) {
	int status = sscanf(line, "Time:%d BG:%3d%3d%3d Keys:%1d%1d%1d%1d%d\n",
			&snaptime, &r, &g, &b, 
			&xmov, &ymov, &xaim, &yaim, &use); 

	return (status == 9);
}

bool Demo::readThing(std::string record) {
	//int x, y, spr, frame, left;
	obj++;
	int status = sscanf(	record.c_str(), "%d %d\t%d %d %d\n",
				&x[obj], &y[obj],
				&spr[obj], &frame[obj], &faceleft[obj]);

	if (status != 5)	return false;

	// catch bad spr
	if (spr[obj] > Globals::SPREND) {
		return false;
	}
	// fix bad frame
	frame[obj] %= globals->spr[spr[obj]]->getFrames();
	
	drawThing();
	return true;
}

void Demo::drawThing() {
	globals->spr[(spr[obj])]->draw(
		(x[obj])<<8, (y[obj])<<8, 
		frame[obj], (bool)(faceleft[obj]));
}

void Demo::drawWorld() {
	SDL_SetRenderDrawColor(game_renderer, ((float)r/100) * 255, ((float)g/100) * 255, ((float)b/100) * 255, 255);
	SDL_RenderClear(game_renderer);
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
