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
#include "Globals.hh"
#include "Sprite.hh"
#include "Sound.hh"
#include "SkillLevel.hh"
#include "Display.hh"
#include <string>
#include <algorithm>
#include <locale>
#include <list>
#include <iostream>

using namespace std;
Globals* globals;

Globals::Globals(std::string resfile) {
	int loadstart = SDL_GetTicks();

	// misc graphics
		smallfont = 0;
		mediumfont = 0;
		largefont = 0;
		sur_testing = 0;

	fullscreen = false;
	vertdisplay = false;
	showloadscreens = true;

	verbosity = 0;
	num_players = 1;
	loadstate = 0;

//	buffer = 0;

	manager = new CL_ResourceManager(resfile);
//	buffer = new CL_InputBuffer();

	// misc graphics
//	try {
		smallfont = new CL_Font("Fonts/smallfont", manager);
		largefont = new CL_Font("Fonts/largefont", manager);
		mediumfont = new CL_Font("Fonts/mediumfont", manager);
		sur_testing = new CL_Surface("Testing/test", manager);
/*	} catch	(CL_Error x) {
		cout << x.message.c_str() << endl;
	}*/
	loadingScreen(loadstart);
}

/*	To add a new sprite requires:
	Adding the resource path to resname
	Adding it to the index enum in Globals.hh
	mention in res.scr
*/
void Globals::loadSprites() {
	int loadstart = SDL_GetTicks();

	/* DO NOT PUT DUPLICATE ENTRIES IN HERE,
	 * Clanlib will give the same SurfaceProvider* each time the same resource is 
	 * loaded. Attempts to delete dangling pointers will follow. Instead, move the
	 * duplication to the resource file. (reference same pcx file from there)
	 *
	 * Also, remember commas, otherwise it'll compile and concatenate it with the 
	 * next entry.
	 */
	
	std::string resname[] = {
		"Surfaces/player1",
		"Surfaces/player2",
		"Surfaces/player1arm",
		"Surfaces/player2arm",
		"Surfaces/singlegrunt", 
		"Surfaces/bullet", 
		"Surfaces/pellet", 
		"Surfaces/trash",
		"Surfaces/crate",
		"Surfaces/forcefield",
		"Surfaces/ball",
		"Surfaces/obstacle",
		"Surfaces/asteroid",
		"Surfaces/sniper",
		"Surfaces/fastsniper",
		"Surfaces/bomb",
		"Surfaces/spiker",
		"Surfaces/rightangler",
		"Surfaces/simpleboss",
		"Surfaces/circleboss",
		"Surfaces/spike",
		"Surfaces/pod",
		"Surfaces/signal",
		"Surfaces/flame",
		"Surfaces/spark",
		"Surfaces/droid",
		"Surfaces/rockman",
		"Surfaces/turret",
		"Surfaces/roundbullet",
		"Surfaces/soldier",
		"Surfaces/drone",
		"Surfaces/centipedesegment",
		"Surfaces/centipedehead",
		"Surfaces/cellphone",
		"Surfaces/spiral",
		"Surfaces/laserhoriz",
		"Surfaces/laservert",
		"Surfaces/goreexplode",
		"Surfaces/metallicexplode",
		"Surfaces/fireexplode",
		"Surfaces/bulletsoaked",
		"Surfaces/reel",
		"Surfaces/bonusbubble",
		"Surfaces/bonus/xlife",
		"Surfaces/bonus/teleport",
		"Surfaces/bonus/shades",
		"Surfaces/bonus/threeway",
		"Surfaces/bonus/rearfire",
		"Surfaces/bonus/flame",
		"Surfaces/bonus/shield",
		"Surfaces/bonus/reflection",
		"Surfaces/bonus/mines",
		"Surfaces/bonus/bomb",
		"Surfaces/bonus/speedup",
		"Surfaces/bonus/shrink",
		"Surfaces/bonus/fruit",
		"Surfaces/bonus/xtime",
		"Surfaces/bonus/repel",
		"Surfaces/bonus/freeze",
		"Surfaces/bonus/slow",
		"Surfaces/bonus/droid",
		"Surfaces/bonus/machinegun",
		"Surfaces/bonus/crate"
	};
	
	for(int ii=0; ii < Globals::SPREND; ii++) {
		spr[ii] = new Sprite(resname[ii], manager);
		if (!(ii%10))	loadingScreen(loadstart);	// update display
	}
}

void Globals::loadingScreen(int loadstarttime) {
	if (showloadscreens) {
		/*loadstate = (loadstate + 1) % MAX_LOADSTATE;
		int margin = (XWINSIZE>>9) - 70;
		int height = (YWINSIZE>>9) - 20;
		CL_Display::fill_rect(0, 0, XWINSIZE>>8, YWINSIZE>>8, 0.12f, 0.08f, 0.3f, 1.0f);
		switch (loadstate) {
			case 0:
				largefont->print_left(margin, height, "Loading");
				break;
			case 1:
				largefont->print_left(margin, height, "Loading.");
				break;
			case 2:
				largefont->print_left(margin, height, "Loading..");
				break;
			case 3:
				largefont->print_left(margin, height, "Loading...");
				break;
		}
		CL_Display::flip_display();*/
//		CL_System::keep_alive();
	}
}

void Globals::fadeScreen(int timetotransition, int length) {
	if (-timetotransition > length) {
		unfade();
		return;
	}

	int yh = (YWINSIZE>>8);

	int pixelsmoved = (abs(yh*(-timetotransition)/length));
	int verticalmove = (yh - pixelsmoved)/2;

	int lowerclip = min(pixelsmoved + yh/2, yh);
/*	CL_ClipRect cr = CL_ClipRect(0, verticalmove, (XWINSIZE>>8), lowerclip);

	CL_Display::set_translate_offset(0, verticalmove);
	CL_Display::set_clip_rect(cr);*/
}

void Globals::unfade() {
/*	CL_ClipRect cr = CL_ClipRect(0, 0, (XWINSIZE>>8), (XWINSIZE>>8));
	CL_Display::set_clip_rect(cr);
	CL_Display::set_translate_offset(0, 0);*/
}

Globals::~Globals() {
	int i;
	for(i=0; i < Globals::SPREND; i++) {
		delete spr[i];
		spr[i] = 0;
	}

	/* Don't think I'm supposed to delete fonts
	delete smallfont;
	delete mediumfont;
	delete largefont;
	*/
	delete sur_testing;
	
//	delete buffer;
	delete manager;
	globals = NULL;
}

void Globals::debugPrint(std::string message, int threshold) {
	if (verbosity > threshold) {
		cout << message << endl;
	}
}

int Globals::loadInt(std::string dname) {
	int x = manager->get_integer_resource(dname, 0);
	if (verbosity > 1)
		cout << dname << " = " << x << endl;
	return x;
}

std::string Globals::loadString(std::string dataname) {
//	try { 
		// x is automatic!?
		std::string x = manager->get_string_resource(dataname, "");
		if (verbosity > 0)
			cout << dataname << " = " << x << endl;
		// must be a deep copy I guess
		return x;
/*	} catch(CL_Error err) {
		if (verbosity > 0)
			cout << err.message.c_str() << " Defaulting to empty." << endl;
		return "";
	}*/
}

CL_Surface* Globals::loadSurface(std::string dataname) {
//	try {
		CL_Surface* s = new CL_Surface(dataname, manager);
		return s;
/*	} catch(CL_Error err) {
		if (verbosity > 0)
			cout << err.message.c_str() << " Defaulting to test." << endl;
		return sur_testing;
	}*/
}
