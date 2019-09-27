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
#include "Intro.hh"
#include "Thing.hh"
#include "Sprite.hh" 
#include "Sound.hh"
#include "Menu.hh"
#include "Display.hh"
#include "Demo.hh"
#include "InputState.hh"

int Intro::timer = 0;
int Intro::restarttime = 0;
int Intro::introstarttime = 0;
int Intro::slomoval = 1;
int Intro::demotime = 0;
int Intro::delta = 0;
int Intro::prevtime = 0;
CL_Surface* Intro::sur_bubble = NULL;
CL_Surface* Intro::sur_starfield = NULL;
CL_Surface* Intro::sur_title = NULL;
CL_Surface* Intro::sur_skillicons = NULL;
Demo* Intro::demo = NULL;
int Intro::fruit[] = { 	51, 51, 51, 
			51, 51, 51, 
			51, 51, 51 };

using namespace std;

extern Globals* globals;

bool Intro::show() {
	demo = new Demo("demo");
	sur_bubble = globals->loadSurface("Surfaces/bubble");
	sur_starfield = globals->loadSurface("Surfaces/stars");
	sur_title = globals->loadSurface("Surfaces/title");
	sur_skillicons = globals->loadSurface("Surfaces/skillicons");

	Sound::playMusic(Sound::TITLE);

	introstarttime = SDL_GetTicks();
	restarttime = introstarttime;
	prevtime = introstarttime;

	Menu m = Menu(Menu::MAIN);
	do {
		int currenttime = SDL_GetTicks();
		delta = currenttime - prevtime;
		prevtime = currenttime;

		timer += delta;

		if (timer < STORYTIME) {
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			if (state[SDL_SCANCODE_SPACE] ||
				state[SDL_SCANCODE_ESCAPE]) {
				timer = STORYTIME;
			}

			drawBg();
			story();
			globals->smallfont->print_center(XWINSIZE>>9, (YWINSIZE>>8) - 25, "Press Space to start");

		} else {
			if (!(m.type == Menu::OPTIONS || m.type == Menu::CONTROLS))
				demoBg();

			Menu::menustatus ret = m.run(delta);
			if (ret == Menu::STARTGAME)	return true;
			else if (ret == Menu::EXIT)	return false;
		}

		InputState::process();
		SDL_RenderPresent(game_renderer);

	} while(true);
}

// starfield, before menu
void Intro::drawBg() {
	int t = SDL_GetTicks();
	SDL_fill_rect(0, 0, XWINSIZE>>8, YWINSIZE>>8, sur_starfield, t/50, t/100);
}

// main menu background stuff
void Intro::demoBg() {
	if (timer < (STORYTIME+700)) {
		globals->fadeScreen(STORYTIME - timer, 700);
	} else {
		globals->unfade();
	}

	// slomo value of zero pauses demoplayback
	if (slomoval)
		demotime += delta/slomoval;
		
	bool more = demo->play(demotime);
	if (!more) demo->restart(demotime);

	sur_title->put_screen(30, 30); 
	globals->smallfont->print_left(30, 100, "} 2001 Craig Timpany");

	if (timer < CONTROLSSTART) {
		slomoval = 1;
	} else if (timer < POWERUPSTART) {
		controlsDemo();
	} else if (timer < FRUITSTART) {
		powerups();
	} else if (timer < ENEMYSTART) {
		fruitTable();
	} else if (timer < RESTART) {
		enemies();
	} else {
		restart();
	}
}

void Intro::restart() {
	introstarttime = SDL_GetTicks();
	timer = STORYTIME;
}

void Intro::story() {
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9) - 100, "Now it is the beginning of");
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9) - 75, "a fantastic story! Let us");
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9) - 50, "make a journey to");
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9) - 25, "the cave of monsters.");
	globals->mediumfont->print_center(XWINSIZE>>9, (YWINSIZE>>9) + 50, "Try hard!");

	globals->spr[Globals::PLAYERONE]->draw(	100<<8, YWINSIZE/2, 
						timer % (globals->spr[Globals::PLAYERONE]->getFrames()), false);
	globals->spr[Globals::PLAYERTWO]->draw(	XWINSIZE - (100<<8), YWINSIZE/2, 
						timer % (globals->spr[Globals::PLAYERTWO]->getFrames()), true);
	sur_bubble->put_screen(90, (YWINSIZE>>9)-15,
				(timer%600) > 300);
	sur_bubble->put_screen((XWINSIZE>>8)-110, (YWINSIZE>>9)-15,
				(timer%600) > 300);
}

void Intro::controlsDemo() {
	int freezeframetime = 2000;
	int slomotime = 1000;
	
	int t = timer - (CONTROLSSTART);

	if (t < slomotime) {
		slomoval = 4;
	} else if (t < slomotime + freezeframetime) {
		slomoval = 0;
		demo->drawAxisPair(0, true, demo->xmov, demo->ymov, 100, 200);
	} else if (t < 2*slomotime + freezeframetime) {
		slomoval = 4;
	} else {
		slomoval = 0;
		demo->drawAxisPair(0, false, demo->xaim, demo->yaim, 420, 200);
	}

	//demo->drawBonus();
}

void Intro::enemies() {
	slomoval = 0;	// pause demo movement
	int ii = 0;

	drawEnemyLine(ii++, Globals::OBSTACLE, "Mushroom", "Poisonous to the touch");
	drawEnemyLine(ii++, Globals::SPIKER, "Bee", "Bursts stingers when killed");
	drawEnemyLine(ii++, Globals::RIGHTANGLER, "Frog", "Sneaks up along the edges and lunges!");
	drawEnemyLine(ii++, Globals::SNIPER, "Rabbit", "Moves fast and breeds rapidly");
	drawEnemyLine(ii++, Globals::ROCKMAN, "Stone Head", "Indestructable");
	drawEnemyLine(ii++, Globals::LASERHORIZ, "Laser", "Indestructable, and periodically shoots beams");
}

void Intro::drawEnemyLine(int line, int s, std::string name, std::string info) {
	int ypos = 200 + line * 50;
	globals->spr[s]->draw(	(100)<<8, (ypos)<<8, 
				(SDL_GetTicks()/100) % globals->spr[s]->getFrames(), false);
	globals->mediumfont->print_left(150, ypos - 10, name.c_str());
	globals->mediumfont->print_left(150, ypos + 10, info.c_str());
}

void Intro::fruitTable() {
	slomoval = 0;
	int rowtime = (ENEMYSTART - FRUITSTART)/3;
	int t = timer - FRUITSTART;

	globals->mediumfont->print_left(100, 150, "Collect fruit for awards");

	if (t < rowtime) {
		if (t < 500)		{ fruit[0] = 11; fruit[1] = 11; fruit[2] = 11; }
		else if (t < 1000)	{ fruit[0] = 12; fruit[1] = 12; fruit[2] = 12; }
		else 			{ fruit[0] = 13; fruit[1] = 13; fruit[2] = 13; }
		globals->mediumfont->print_left(350, 200, "Three in a row!");
	} else if (t < 2*rowtime) {
		if (t < rowtime + 500)		{ fruit[3] = 11; fruit[4] = 12; fruit[5] = 13; }
		else if (t < rowtime + 1000)	{ fruit[3] = 31; fruit[4] = 32; fruit[5] = 33; }
		else 				{ fruit[3] = 51; fruit[4] = 52; fruit[5] = 53; }
		globals->mediumfont->print_left(350, 275, "Three of a colour!");
	} else {
		if (t < 2*rowtime + 500)	{ fruit[6] = 31; fruit[7] = 42; fruit[8] = 53; }
		else if (t < 2*rowtime + 1000)	{ fruit[6] = 21; fruit[7] = 32; fruit[8] = 43; }
		else 				{ fruit[6] = 11; fruit[7] = 22; fruit[8] = 33; }
		globals->mediumfont->print_left(350, 350, "Rainbow colours");
	}

	for (int ii=0; (ii*rowtime < t) && ii < 3 ; ii++) {
		for (int jj=0; jj < 3; jj++) {
			Sprite* reel = globals->spr[Globals::REEL];

			int x = jj * reel->width;
			x += (150 + jj * 2)<<8;

			int y = (200 + 75*ii)<<8;

			reel->draw(x, y, 0, false);
			globals->spr[Globals::BONUSFRUIT]->
				draw(x, y, fruit[ii*3 + jj], false);
		}
	}
}

void Intro::powerups() {
	slomoval = 0;

	globals->mediumfont->print_left(160, 200, "Grab items:");

	int i = (SDL_GetTicks()) / 500;
	int powerups = Globals::SPREND - Globals::BONUSXLIFE;
	i %= powerups;
	i += Globals::BONUSXLIFE;
	int xpos = (180)<<8;	int ypos = (240)<<8;
	globals->spr[i]->draw(xpos, ypos, 0, false);

	Sprite* bubble = globals->spr[Globals::BONUSBUBBLE];
	int centering = bubble->width/2 - globals->spr[Globals::BONUSXLIFE]->width/2;
	bubble->draw(xpos - centering, ypos - centering, i%2, false);

	if (timer > POWERUPSTART + 2000) {
		globals->mediumfont->print_right(350, 200, "Then ");
		demo->drawBonus(0, 350, 200);
	}
}

void Intro::deinit() {
	delete sur_bubble;	sur_bubble = NULL;
	delete sur_starfield;	sur_starfield = NULL;
	delete sur_title;	sur_title = NULL;
	delete sur_skillicons;	sur_skillicons = NULL;
	delete demo;		demo = NULL;
}
