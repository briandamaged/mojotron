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
#include <numbers>
#include <cmath>

extern Globals* globals;

IntroPlayer::IntroPlayer(int x1, int y1, int p) : IntroThing(x1, y1, 0, globals->spr[p]->anim), player(p), armanim(globals->spr[Globals::MONKEYARMONE + p]->anim) {
}

void IntroPlayer::act(int timer) {
	timer = timer / 10;
	frame = timer % (anim.get_num_frames() / 2);
	if (player == Globals::PLAYERONE)
		frame += anim.get_num_frames() / 2;
}

void IntroPlayer::draw() {
	IntroThing::draw();
	if (!armanim.is_null()) {
		armanim.put_screen(x, y, 3 + (player ? 0 : 5));
	}
}

const std::string IntroHuman::typesurface[3] = { "Intro/man", "Intro/woman", "Intro/child" };

IntroHuman::IntroHuman(int x1, int y1, int t) : IntroThing(x1, y1, 0, CL_Surface(typesurface[t], globals->manager)), type(t) {
}

void IntroHuman::act(int timer) {
	timer = timer / 100;
	frame = timer % (anim.get_num_frames() / 4);
	switch (type) {
		case 0:
			frame += anim.get_num_frames() / 4 * 3;
			break;
		case 1:
			frame += anim.get_num_frames() / 4;
			break;
		case 2:
			frame += anim.get_num_frames() / 4 * 2;
			break;
		default:
			break;
	}
}

IntroBubble::IntroBubble(int x1, int y1) : IntroThing(x1, y1, 0, CL_Surface("Surfaces/bubble", globals->manager)) {
}

void IntroBubble::act(int timer) {
	frame = (timer % 600) > 300;
}

const std::string IntroRobot::typesurface[3] = { "Surfaces/aolcd", "Surfaces/cjt", "Surfaces/ghost" };

IntroRobot::IntroRobot(int x1, int y1, int t) : IntroThing(x1, y1, 0, CL_Surface(typesurface[t], globals->manager)), type(t), lastact(-1) {
	double angle = ((float)rand() / RAND_MAX) * 2.0 * std::numbers::pi;
	x = (XWINSIZE >> 9) + (x / 2) * std::cos(angle) - anim.get_width() / 2;
	y = (YWINSIZE >> 9) + (y / 2) * std::sin(angle) - anim.get_height() / 2;
}

void IntroRobot::act(int timer) {
	if (lastact == -1)
		lastact = timer;
	if ((timer - lastact) / 100 > 0)
	{
		int xdiff = (XWINSIZE >> 9) - x;
		int ydiff = (YWINSIZE >> 9) - y;
		if (abs(xdiff) > abs(ydiff)) {
			if (xdiff > 0)
				x++;
			else
				x--;
		}
		else {
			if (ydiff > 0)
				y++;
			else
				y--;
		}
		lastact = timer;
	}
	timer = timer / 100;
	frame = timer % anim.get_num_frames();
}

IntroThing::IntroThing(int x1, int y1, int f, CL_Surface a): x(x1), y(y1), frame(f), anim(a) {
}

IntroThing::~IntroThing() {
}

void IntroThing::draw() {
	anim.put_screen(x, y, frame);
}

IntroSpecification::IntroSpecification() {
}

void IntroSpecification::load(std::string prefix) {
	type = globals->loadInt(prefix + "type");
	x = globals->loadInt(prefix + "x");
	y = globals->loadInt(prefix + "y");
	data = globals->loadInt(prefix + "data");
}

IntroThing* IntroSpecification::instantiate() {
	switch (type) {
		case 1:
			return new IntroPlayer(x, y, data);
			break;
		case 2:
			return new IntroBubble(x, y);
			break;
		case 3:
			return new IntroHuman(x, y, data);
			break;
		case 4:
			return new IntroRobot(x, y, data);
			break;
		default:
			break;
	}
	return NULL;
}

int Intro::timer = 0;
int Intro::restarttime = 0;
int Intro::introstarttime = 0;
int Intro::slomoval = 1;
int Intro::demotime = 0;
int Intro::delta = 0;
int Intro::prevtime = 0;

int Intro::storytime = 9000;

std::string Intro::subtitle;
CL_Surface* Intro::sur_skillicons = NULL;
CL_Surface* Intro::sur_starfield = NULL;
Demo* Intro::demo = NULL;
int Intro::fruit[] = { 	51, 51, 51, 
			51, 51, 51, 
			51, 51, 51 };
int Intro::textstart;
int Intro::textincrement;

using namespace std;

Intro::Intro() {
	sur_bubble = NULL;
	sur_title = NULL;
	currentscene = -1;
	std::string base = "Intro/scene0/";
	std::string label = "Intro/scene0/text1";
	storytime = 0;
	textstart = globals->loadInt("Intro/textstart");
	textincrement = globals->loadInt("Intro/textincrement");
	for (int scene = 1; scene < 9; scene++) {
		base[11] = '0' + scene;
		label[11] = '0' + scene;
		label[17] = '1';
		if (globals->manager->resource_exists(label)) {
			int texty = textstart;
			scenes.push_back(IntroScene());
			for (int i = 1; i < 9; i++) {
				label[17] = '0' + i;
				if (globals->manager->resource_exists(label)) {
					scenes.back().text.push_back(globals->loadString(label));
					std::string skiplabel = base + "skip1";
					skiplabel[17] = '0' + i;
					if (globals->manager->resource_exists(skiplabel)) {
						texty = globals->loadInt(skiplabel);
					}
					scenes.back().texty.push_back(texty);
					texty += textincrement;
				}
				else
					break;
			}
			storytime += globals->loadInt(base + "storytime");
			scenes.back().storytime = storytime;
			std::string thinglabel = base + "thing01/";
			while (globals->manager->resource_exists(thinglabel + "type")) {
				scenes.back().specs.push_back(IntroSpecification());
				scenes.back().specs.back().load(thinglabel);
				if (thinglabel[19] == '9') {
					thinglabel[18] = thinglabel[18] + 1;
					thinglabel[19] = '0';
				}
				else
					thinglabel[19] = thinglabel[19] + 1;
			}
		}
	}
}

Intro::~Intro() {
	delete sur_bubble;	sur_bubble = NULL;
	delete sur_starfield;	sur_starfield = NULL;
	delete sur_title;	sur_title = NULL;
	delete sur_skillicons;	sur_skillicons = NULL;
	delete demo;		demo = NULL;
}

bool Intro::show() {
	if (demo == NULL) {
		demo = new Demo("demo");
		sur_bubble = globals->loadSurface("Surfaces/bubble");
		sur_starfield = globals->loadSurface("Surfaces/stars");
		sur_title = globals->loadSurface("Surfaces/title");
		sur_skillicons = globals->loadSurface("Surfaces/skillicons");
		sur_view = CL_Surface("Intro/smallviewbg", globals->manager);
		sur_viewport = CL_Surface("Intro/smallviewport", globals->manager);
		subtitle = globals->loadString("Intro/subtitle");
	}

	Sound::playMusic(Sound::TITLE);

	introstarttime = SDL_GetTicks();
	restarttime = introstarttime;
	prevtime = introstarttime;

	Menu m(Menu::MAIN);
	do {
		int currenttime = SDL_GetTicks();
		delta = currenttime - prevtime;
		prevtime = currenttime;

		timer += delta;

		if (timer >= RESTART) {
			restart(m.checkActive());
		}
		if (timer < STORYTIME) {
			if ((currentscene == -1) || (timer > scenes[currentscene].storytime))
			{
				currentscene++;
				things.clear();
				for (auto &s : scenes[currentscene].specs) {
					things.push_back(std::unique_ptr<IntroThing>(s.instantiate()));
				}
			}
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			if (state[SDL_SCANCODE_SPACE] ||
				state[SDL_SCANCODE_ESCAPE]) {
				timer = STORYTIME;
				m.readyforkey = false;
			}

			introBg();
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
	SDL_SetRenderDrawColor(game_renderer, 0, 0, 0, 255);
	SDL_RenderClear(game_renderer);
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
	globals->smallfont->print_left(30, 100, subtitle);

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
	}
}

void Intro::introBg() {
	if (sur_view.is_null())
		drawBg();
	else
	{
		SDL_SetRenderDrawColor(game_renderer, 0, 0, 0, 255);
		SDL_RenderClear(game_renderer);
		sur_view.put_screen((XWINSIZE >> 9) - (sur_view.get_width() / 2), (YWINSIZE >> 9) - (sur_view.get_height() / 2), 0);
	}
}

void Intro::restart(bool active) {
	introstarttime = SDL_GetTicks();
	if (active)
		timer = STORYTIME;
	else {
		currentscene = -1;
		timer = 0;
	}
}

void Intro::story() {
	auto textpos = scenes[currentscene].texty.begin();
	for (auto &s : scenes[currentscene].text) {
		globals->mediumfont->print_center(XWINSIZE>>9, *textpos, s);
		textpos++;
	}
	for (auto &t : things) {
		t->act(timer);
		t->draw();
	}
	if (!sur_viewport.is_null())
		sur_viewport.put_screen((XWINSIZE >> 9) - (sur_viewport.get_width() / 2), (YWINSIZE >> 9) - (sur_viewport.get_height() / 2), 0);
	int left = scenes[currentscene].storytime - timer;
	if (left < 1000) {
		SDL_BlendMode bm;
		if (SDL_GetRenderDrawBlendMode(game_renderer, &bm) == 0) {
			SDL_SetRenderDrawBlendMode(game_renderer, SDL_BLENDMODE_BLEND);
			SDL_Rect r;
			SDL_SetRenderDrawColor(game_renderer, 0, 0, 0, ((1000 - left) / 1000.0) * 255);
			r.x = 0;
			r.y = 0;
			r.w = XWINSIZE>>8;
			r.h = YWINSIZE>>8;
			SDL_RenderFillRect(game_renderer, &r);
			SDL_SetRenderDrawBlendMode(game_renderer, bm);
		}
	}
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
