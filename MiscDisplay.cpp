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

#include <ClanLib/core.h>
#include "MiscDisplay.hh"
#include "World.hh"
#include "Display.hh"

using namespace std;
extern World* worldobj;
extern Globals* globals;

CL_Surface* MiscDisplay::livesicon = NULL;
CL_Surface* MiscDisplay::levelicon = NULL;
CL_Surface* MiscDisplay::timeicon = NULL;

MiscDisplay::MiscDisplay() {
	consoletimer = 0;
	livesicon = globals->loadSurface("Surfaces/livesicon");
	levelicon = globals->loadSurface("Surfaces/levelicon");
	timeicon = globals->loadSurface("Surfaces/timeicon");
}

MiscDisplay::~MiscDisplay() {
	delete livesicon;
	delete levelicon;
	delete timeicon;
}

void MiscDisplay::drawMisc() {
	if (globals->vertdisplay) {
		drawLevel(0, (YWINSIZE>>8) - 3*DISPHEIGHT - 4);
		drawLives(0, (YWINSIZE>>8) - 2*DISPHEIGHT - 4);
		drawClock(0, (YWINSIZE>>8) - DISPHEIGHT - 4);
	} else {
		int xorigin = 8;
		int yorigin = (YWINSIZE>>8) - DISPHEIGHT - 4;
		int width = 200;	// about total display width
		drawLevel(xorigin, yorigin);
		drawLives(xorigin+(width/3), yorigin);
		drawClock(xorigin+(3*width/4), yorigin); 
	}
}

void MiscDisplay::drawLevWarp(int xlives, int skipped, int time) {
	int x = XWINSIZE>>9;
	int y = (YWINSIZE>>9)-16;

	globals->largefont->print_center(x, y-32, "Level Warp");
	if ((time%600) > 300 || time > 1000) {
		drawClock(x-32, y-64);
	}

	char str[3];
	if (time > 1000) {
		sprintf(str, "+%d", skipped);
		globals->largefont->print_left(x + 32, y, str);
		levelicon->put_screen(x, y);
	}
	
	if (time > 2000) {
		y += 32;
		sprintf(str, "+%d", xlives);
		globals->largefont->print_left(x + 32, y, str);
		livesicon->put_screen(x, y);
	}
}

void MiscDisplay::drawHelpBonus(int time) {
	int x = XWINSIZE>>9;
	int y = (YWINSIZE>>9)-16;

	char str[3];
	globals->largefont->print_center(x, y-32, "Help Bonus");
	if (time > 1000) {
		sprintf(str, "+1");
		globals->largefont->print_left(x + 32, y, str);
		livesicon->put_screen(x, y);
	}
}

void MiscDisplay::drawWonGame() {
	globals->largefont->print_center(XWINSIZE>>9, (YWINSIZE>>9)-150, "404 No More Game!!");
	//globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)-100, "This is the file not found message of GLORY.");
	//globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)-75, "Mojotron can't find another level, so you win!");
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)-75, "Mojo the monkey lived happily ever after, and was");
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)-50, "thanked by everyone for clearing the land of monsters.");
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9), "Everyone except those stone head things, who still");
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)+25, "want his blood. You can't please everybody, I suppose.");
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)+150, "Press any key to continue.");
}

void MiscDisplay::drawClock(int xorigin, int yorigin) {
	int time = worldobj->getWarpRemaining();
	char timestr[4];
	if (time > -1)	sprintf(timestr, ":%d", time);
	else		sprintf(timestr, " - ");
	globals->largefont->print_left(xorigin + 32, yorigin, timestr);
	timeicon->put_screen(xorigin, yorigin);
}

void MiscDisplay::drawLevel(int xorigin, int yorigin) {
	char levstr[3];
	sprintf(levstr, "%d", worldobj->num_lev);
	globals->largefont->print_left(xorigin + 32, yorigin, levstr);
	levelicon->put_screen(xorigin, yorigin);
}

void MiscDisplay::drawLives(int xorigin, int yorigin) {
	char livstr[4];
	sprintf(livstr, "x%d", worldobj->lives);
	globals->largefont->print_left(xorigin + 32, yorigin, livstr);
	livesicon->put_screen(xorigin, yorigin);
}

void MiscDisplay::drawConsole() {
	if (consoletimer) {
		consoletimer -= worldobj->delta;
		if (consoletimer < 0) consoletimer = 0;
		globals->smallfont->print_center((ARENAWIDTH/2)>>8, 0, consoledisplay.c_str());
	}
}

void MiscDisplay::setConsole(std::string newstr, int duration) {
	consoledisplay = newstr;	
	consoletimer = duration;
}


