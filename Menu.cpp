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
#include "Menu.hh"
#include "Intro.hh"
#include "Sprite.hh"
#include "Sound.hh"
#include "SkillLevel.hh"
#include "Display.hh"
#include "InputState.hh"
#include "Demo.hh"

using namespace std;
extern Globals* globals;

Menu::Menu(menutype t) {
	changeType(t);
	prevtype = NOMENU;

	choosingkey = false;
	active = false;
//	keyidpicked = CL_NO_KEY;
}

Menu::~Menu() {
}

void Menu::changeType(menutype t) {
	msgline = "Use the cursor keys to change settings.";
	currententry = 0;
	type = t;
	slots.clear();
	switch (t) {
		case MAIN:
			slots.push_back(std::make_unique<MenuSlot>("1 Player Game", &start1Game, 0));

			slots.push_back(std::make_unique<MenuSlot>("2 Player Game", &start2Game, 1));

			slots.push_back(std::make_unique<MenuSlot>("Options", &optionsMenu, 1));

			slots.push_back(std::make_unique<MenuSlot>("Controls", &controlsMenu, 1));

			slots.push_back(std::make_unique<MenuSlot>("Quit", &exitMenu, 1));

			maxentries = 5;
			prevtype = NOMENU;
			break;

		case OPTIONS:
			slots.push_back(std::make_unique<BooleanSlot>("Fullscreen", &globals->fullscreen, &Menu::toggleFullscreen));
			slots.push_back(std::make_unique<BooleanSlot>("Sounds", &Sound::sound, &Menu::toggleSound));
			slots.push_back(std::make_unique<BooleanSlot>("Music", &Sound::music, &Menu::toggleMusic));
			slots.push_back(std::make_unique<SkillSlot>(2));

			slots.push_back(std::make_unique<MenuSlot>("Back", &exitMenu, 2));
			maxentries = 5;
			break;

		case CONTROLS: 
			slots.push_back(std::make_unique<KeysSlot>(0, true));
			slots.push_back(std::make_unique<KeysSlot>(0, false));
			slots.push_back(std::make_unique<UseKeySlot>(0));
			slots.push_back(std::make_unique<KeysSlot>(1, true));
			slots.push_back(std::make_unique<KeysSlot>(1, false));
			slots.push_back(std::make_unique<UseKeySlot>(1));

			slots.push_back(std::make_unique<MenuSlot>("Back", &exitMenu, 2));
			maxentries = 7;
			break;

		case INGAME:
			slots.push_back(std::make_unique<MenuSlot>("Resume Game", &start1Game, 0));

			slots.push_back(std::make_unique<MenuSlot>("Options", &optionsMenu, 1));

			slots.push_back(std::make_unique<MenuSlot>("Controls", &controlsMenu, 1));

			slots.push_back(std::make_unique<MenuSlot>("Quit Game", &exitMenu, 1));

			maxentries = 4;
			prevtype = NOMENU;
			break;

		default:
			break;
	}
	status = NONE;
	readyforkey = false;
}

Menu::menustatus Menu::run(int delta) {
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	bool moveup = state[SDL_SCANCODE_UP];
	bool movedown = state[SDL_SCANCODE_DOWN];
	bool select = 	state[SDL_SCANCODE_SPACE] ||
			state[SDL_SCANCODE_RETURN] ||
			state[SDL_SCANCODE_RIGHT];
	bool back = state[SDL_SCANCODE_LEFT];

	if (!choosingkey) {
		if (readyforkey) {
			if (moveup) {
				currententry--;
				if (currententry < 0)	currententry = maxentries - 1;
			} else if (movedown) {
				currententry++;
				currententry %= maxentries;
			} else {
				if (select)
					slots[currententry]->activatefunc(true, this, slots[currententry]->player, slots[currententry]->movement);
				else if (back)
					slots[currententry]->activatefunc(false, this, slots[currententry]->player, slots[currententry]->movement);
			}
		}
		readyforkey = !(moveup || movedown || select || back);
		if (!readyforkey)
			active = true;
	} else {
		active = true;
		if (!(select || back) && !readyforkey) {
			readyforkey = true;
			InputState::anyKeyPress();
		}
		if (state[SDL_SCANCODE_ESCAPE]) {
			choosingkey = false;

		} else if (readyforkey) {
			if (InputState::anyKeyPress()) {
				keyidpicked = InputState::getLastScancode();
        
				InputState::anyKeyPress();
				slots[currententry]->activatefunc(true, this, slots[currententry]->player, slots[currententry]->movement);
				readyforkey = false;
			}
		}
	}

	lasttime = SDL_GetTicks();
	draw();

	return status;
}

bool Menu::checkActive() {
	bool result = active;
	active = false;
	return result;
}

void Menu::draw() {
	int startx = 75;
	int starty = 130;
	int offset = 0;
	int border = 10;

	switch (type) {
		case MAIN:
		case INGAME:
			starty = 30;
			offset = 300;
			SDL_Rect r;
			SDL_SetRenderDrawColor(game_renderer, 0, 0, 0, 255);
			r.x = (XWINSIZE>>9) + offset - NAMEWIDTH - border;
			r.y = starty - border;
			r.w = NAMEWIDTH + 2 * border;
			r.h = LINESPACE * maxentries + 2 * border;
			SDL_RenderFillRect(game_renderer, &r);
			break;
		case CONTROLS:
		case OPTIONS:
			Intro::drawBg();
			offset = 100;
			globals->largefont->print_left(startx, starty/2, "Mojotron Options");
			globals->mediumfont->print_left(startx, starty/2 + 40, msgline);
			break;
		default:
			break;
	}

	int ypos = starty;
	int xpos = (XWINSIZE>>9) + offset;

	for (int i = 0; i < maxentries; i++) {
		bool selected = (i == currententry);
		slots[i]->draw(&xpos, &ypos, selected);
	}
}

void Menu::toggleSound(bool forward, Menu* m, int player, bool movement) {
	Sound::setSFX(!(Sound::sound));
	Sound::playSound("takebonus");
}

void Menu::toggleMusic(bool forward, Menu* m, int player, bool movement) {
	if (Sound::music) {
		Sound::setMusic(false);
	} else {
		Sound::setMusic(true);
		Sound::playMusic(Sound::TITLE);
	}
}

void Menu::toggleFullscreen(bool forward, Menu* m, int player, bool movement) {
	globals->fullscreen = !globals->fullscreen;
}

void Menu::exitMenu(bool forward, Menu* m, int player, bool movement) {
	if (m->prevtype != NOMENU) {
		m->changeType(m->prevtype);
	} else {
		m->status = EXIT;
	}
}

void Menu::start1Game(bool forward, Menu* m, int player, bool movement) {
	m->status = STARTGAME;
	globals->num_players = 1;
}

void Menu::start2Game(bool forward, Menu* m, int player, bool movement) {
	m->status = STARTGAME;
	globals->num_players = 2;
}

void Menu::optionsMenu(bool forward, Menu* m, int player, bool movement) {
	m->prevtype = m->type;
	m->changeType(OPTIONS);
}

void Menu::controlsMenu(bool forward, Menu* m, int player, bool movement) {
	m->prevtype = m->type;
	m->changeType(CONTROLS);
}

MenuSlot::MenuSlot(std::string n, MenuActivateFunc a, int l) {
	activatefunc = a;
	name = n;
	linespacing = l;
}

void MenuSlot::draw(int* xpos, int* ypos, bool selected) {
	*ypos += linespacing * LINESPACE;
	// draw the name of the variable
	globals->smallfont->print_right(*xpos, *ypos, name.c_str());

	if (selected && (SDL_GetTicks()%500) > 200) {
		globals->smallfont->print_left(*xpos - NAMEWIDTH, *ypos, ">");
	}
}

BooleanSlot::BooleanSlot(std::string n, bool* b, MenuActivateFunc a) : MenuSlot(n, a, 1) {
	state = b;
}

void BooleanSlot::draw(int* xpos, int* ypos, bool selected) {
	MenuSlot::draw(xpos, ypos, selected);

	std::string strstate;
	if (*state)	strstate = "On";
	else		strstate = "Off";
	globals->smallfont->print_left(*xpos + 30, *ypos, strstate.c_str());
}

KeysSlot::KeysSlot(int p, bool m) : MenuSlot((m ? "Movement" : "Aiming"), &activate, 1) {
	player = p;
	movement = m;
}

void KeysSlot::draw(int* xpos, int* ypos, bool selected) {
	if (movement) {
		*ypos += 2*LINESPACE;	// empty line after last section

		globals->spr[player]->draw((*xpos - NAMEWIDTH)<<8, (*ypos - 10)<<8, 0, false);
		char sectiontitle[20];
		sprintf(sectiontitle, "Player %d Keys:", player+1);
		globals->smallfont->print_right(*xpos, *ypos, sectiontitle);
	}
	MenuSlot::draw(xpos, ypos, selected);

	std::string strstate = 	InputState::keygroups[
					InputState::playercontrols[player]->
					getKeyAxisPair(movement)
				];
	globals->smallfont->print_left(*xpos + 30, *ypos, strstate.c_str());

	if (selected) {	
		Intro::demo->drawAxisPair(player, movement, 
			1, 1, 75, 200);
	}
}

void KeysSlot::activate(bool forward, Menu *m, int player, bool movement) {
	InputState* i = InputState::playercontrols[player];
	int group = i->getKeyAxisPair(movement);

	if (forward)	group++;
	else		group--;

	// wrap around
	group %= InputState::numkeygroups;
	if (group < 0)	
		group = InputState::numkeygroups - 1;

	i->setupKeyAxisPair(group, movement);
}

UseKeySlot::UseKeySlot(int p) : MenuSlot("Use Item", &activate, 1) {
	player = p;
}

void UseKeySlot::draw(int* xpos, int* ypos, bool selected) {
	MenuSlot::draw(xpos, ypos, selected);

	std::string key = InputState::playercontrols[player]->getUseKey();
	globals->smallfont->print_left(*xpos + 30, *ypos, key.c_str());

	if (selected) {	
		Intro::demo->drawBonus(player, 75, 200);
	}
}

void UseKeySlot::activate(bool forward, Menu *m, int player, bool movement) {
	if (m->choosingkey) {
		// found key
		m->choosingkey = false;
		m->msgline = "Use the cursor keys to change settings.";

		// assign keyidpicked
		if (m->keyidpicked) {
			InputState::playercontrols[player]->
				setUseKey(m->keyidpicked);
		}
	} else {
		// going to read a new key from user
		m->choosingkey = true;
		m->readyforkey = false;	// don't read the enter they just pressed
		InputState::anyKeyPress();
		m->msgline = "Press a key to use";
	}
}

SkillSlot::SkillSlot(int l) : MenuSlot("Difficulty", &activate, l) {
}

void SkillSlot::draw(int* xpos, int* ypos, bool selected) {
	MenuSlot::draw(xpos, ypos, selected);

	std::string strstate = SkillLevel::current->name;

	for (int ii=0; ii < MAX_SKILL_LEVS; ii++) {
		if (ii == SkillLevel::current->level)
			Intro::sur_skillicons->put_screen(*xpos + 30, *ypos - 3, ii);

		globals->smallfont->print_left(*xpos + 35 + Intro::sur_skillicons->get_width(),
						*ypos, SkillLevel::skills[ii]->name.c_str());
		*ypos += LINESPACE;
	}
	*ypos -= LINESPACE;
}

void SkillSlot::activate(bool forward, Menu *m, int player, bool movement) {
	SkillLevel::cycleSkill(forward);
}
