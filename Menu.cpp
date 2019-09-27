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
//	keyidpicked = CL_NO_KEY;
}

void Menu::changeType(menutype t) {
	msgline = "Use the cursor keys to change settings.";
	currententry = 0;
	type = t;
	switch (t) {
		case MAIN:
			slots[0] = new MenuSlot();
			slots[0]->name = "1 Player Game";
			slots[0]->activatefunc = &start1Game;
			slots[0]->linespacing = 0;

			slots[1] = new MenuSlot();
			slots[1]->name = "2 Player Game";
			slots[1]->activatefunc = &start2Game;

			slots[2] = new MenuSlot();
			slots[2]->name = "Options";
			slots[2]->activatefunc = &optionsMenu;

			slots[3] = new MenuSlot();
			slots[3]->name = "Controls";
			slots[3]->activatefunc = &controlsMenu;

			slots[4] = new MenuSlot();
			slots[4]->name = "Quit";
			slots[4]->activatefunc = &exitMenu;

			slots[0]->owner = slots[1]->owner = slots[2]->owner = slots[3]->owner = slots[4]->owner = this;
			maxentries = 5;
			prevtype = NOMENU;
			break;

		case OPTIONS:
			slots[0] = new BooleanSlot(&globals->fullscreen, *(Menu::toggleFullscreen));
			slots[0]->name = "Fullscreen";
			slots[1] = new BooleanSlot(&Sound::sound, *(Menu::toggleSound));
			slots[1]->name = "Sounds";
			slots[2] = new BooleanSlot(&Sound::music, *(Menu::toggleMusic));
			slots[2]->name = "Music";
			slots[3] = new SkillSlot();
			slots[3]->linespacing = 2;

			slots[4] = new MenuSlot();
			slots[4]->name = "Back";
			slots[4]->activatefunc = &exitMenu;
			slots[4]->owner = this;
			slots[4]->linespacing = 2;
			maxentries = 5;
			break;

		case CONTROLS: 
			slots[0] = new KeysSlot(0, true);
			slots[1] = new KeysSlot(0, false);
			slots[2] = new UseKeySlot(0, this);
			slots[3] = new KeysSlot(1, true);
			slots[4] = new KeysSlot(1, false);
			slots[5] = new UseKeySlot(1, this);

			slots[6] = new MenuSlot();
			slots[6]->name = "Back";
			slots[6]->activatefunc = &exitMenu;
			slots[6]->owner = this;
			slots[6]->linespacing = 2;
			maxentries = 7;
			break;

		case INGAME:
			slots[0] = new MenuSlot();
			slots[0]->name = "Resume Game";
			slots[0]->activatefunc = &start1Game;
			slots[0]->linespacing = 0;

			slots[1] = new MenuSlot();
			slots[1]->name = "Options";
			slots[1]->activatefunc = &optionsMenu;

			slots[2] = new MenuSlot();
			slots[2]->name = "Controls";
			slots[2]->activatefunc = &controlsMenu;

			slots[3] = new MenuSlot();
			slots[3]->name = "Quit Game";
			slots[3]->activatefunc = &exitMenu;

			slots[0]->owner = slots[1]->owner = slots[2]->owner = slots[3]->owner = this;
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
//				try {
					if (select)
						slots[currententry]->activate(true);
					else if (back)
						slots[currententry]->activate(false);
/*				} catch (CL_Error err) {
					msgline = err.message;
				}*/
			}
		}
		readyforkey = !(moveup || movedown || select || back);
	} else {
		if (!(select || back) && !readyforkey) {
			readyforkey = true;
//			buffer.get_key();	// trash first key
		}
/*		if (CL_Keyboard::get_keycode(CL_KEY_ESCAPE)) {
			buffer.clear();
			choosingkey = false;

		} else if (readyforkey) {
			if (buffer.keys_left() == 1) {
				CL_Key in = buffer.get_key();
				keyidpicked = in.id;
        
				buffer.clear();
				slots[currententry]->activate(true);
				readyforkey = false;
			}
		}*/
	}

	lasttime = SDL_GetTicks();
	draw();

	return status;
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

void Menu::saveOptions() {

}

void Menu::toggleSound() {
	Sound::setSFX(!(Sound::sound));
	Sound::playSound("takebonus");
}

void Menu::toggleMusic() {
	if (Sound::music) {
		Sound::setMusic(false);
	} else {
		Sound::setMusic(true);
		Sound::playMusic(Sound::TITLE);
	}
}

void Menu::toggleFullscreen() {
	globals->fullscreen = !globals->fullscreen;
/*	try {
		CL_Display::set_videomode(XWINSIZE >> 8, YWINSIZE >> 8, 16, globals->fullscreen);
	} catch (CL_Error err) {
		globals->fullscreen = false;
		CL_Display::set_videomode(XWINSIZE >> 8, YWINSIZE >> 8, 16, false);
		cout << "Got error while trying to switch modes: " << err.message << endl;
		throw err;
	}*/
}

void Menu::exitMenu(Menu* m) {
	if (m->prevtype != NOMENU) {
		m->changeType(m->prevtype);
	} else {
		m->status = EXIT;
	}
}

void Menu::start1Game(Menu* m) {
	m->status = STARTGAME;
	globals->num_players = 1;
}

void Menu::start2Game(Menu* m) {
	m->status = STARTGAME;
	globals->num_players = 2;
}

void Menu::optionsMenu(Menu* m) {
	m->prevtype = m->type;
	m->changeType(OPTIONS);
}

void Menu::controlsMenu(Menu* m) {
	m->prevtype = m->type;
	m->changeType(CONTROLS);
}

MenuSlot::MenuSlot() {
	activatefunc = NULL;
	owner = NULL;
	name = "unset";
	linespacing = 1;
}

void MenuSlot::activate(bool forward) {
	activatefunc(owner);
}

void MenuSlot::draw(int* xpos, int* ypos, bool selected) {
	*ypos += linespacing * LINESPACE;
	// draw the name of the variable
	globals->smallfont->print_right(*xpos, *ypos, name.c_str());

	if (selected && (SDL_GetTicks()%500) > 200) {
		globals->smallfont->print_left(*xpos - NAMEWIDTH, *ypos, ">");
	}
}

BooleanSlot::BooleanSlot(bool* b, void (*f)()) : MenuSlot() {
	togglefunc = f;
	state = b;
}

void BooleanSlot::draw(int* xpos, int* ypos, bool selected) {
	MenuSlot::draw(xpos, ypos, selected);

	std::string strstate;
	if (*state)	strstate = "On";
	else		strstate = "Off";
	globals->smallfont->print_left(*xpos + 30, *ypos, strstate.c_str());
}

void BooleanSlot::activate(bool forward) {
	togglefunc();
}

KeysSlot::KeysSlot(int p, bool m) : MenuSlot() {
	player = p;
	movement = m;
	if (movement)	name = "Movement";
	else		name = "Aiming";
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

void KeysSlot::activate(bool forward) {
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

UseKeySlot::UseKeySlot(int p, Menu* m) : MenuSlot() {
	owner = m;
	player = p;
	name = "Use Item";
}

void UseKeySlot::draw(int* xpos, int* ypos, bool selected) {
	MenuSlot::draw(xpos, ypos, selected);

	std::string key = InputState::playercontrols[player]->getUseKey();
	globals->smallfont->print_left(*xpos + 30, *ypos, key.c_str());

	if (selected) {	
		Intro::demo->drawBonus(player, 75, 200);
	}
}

void UseKeySlot::activate(bool forward) {
	if (owner->choosingkey) {
		// found key
		owner->choosingkey = false;
		owner->msgline = "Use the cursor keys to change settings.";

		// assign keyidpicked
		if (owner->keyidpicked) {
			InputState::playercontrols[player]->
				setUseKey(owner->keyidpicked);
		}
	} else {
		// going to read a new key from user
		owner->choosingkey = true;
		owner->readyforkey = false;	// don't read the enter they just pressed
//		owner->buffer.clear();
		owner->msgline = "Press a key to use";
	}
}

SkillSlot::SkillSlot() : MenuSlot() {
	name = "Difficulty";
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

void SkillSlot::activate(bool forward) {
	SkillLevel::cycleSkill(forward);
}
