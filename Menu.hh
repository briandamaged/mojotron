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

#ifndef MENU_HH
#define MENU_HH

#define LINESPACE 20
#define NAMEWIDTH 150

#include <string>

class Menu;

struct MenuSlot {
	MenuSlot();
	virtual ~MenuSlot() { }
	virtual void activate(bool forward);
	virtual void draw(int* xpos, int* ypos, bool selected);
	std::string name;
	void (*activatefunc)(Menu* m);
	Menu* owner;
	int linespacing;
};

struct BooleanSlot : MenuSlot {
	BooleanSlot(bool* b, void (*f)() );
	void activate(bool forward);
	void draw(int* xpos, int* ypos, bool selected);
	bool* state;
	void (*togglefunc)(void);
};

struct KeysSlot : MenuSlot {
	KeysSlot(int p, bool m);
	void activate(bool forward);
	void draw(int* xpos, int* ypos, bool selected);
	int player;
	bool movement;
};

struct UseKeySlot : MenuSlot {
	UseKeySlot(int p, Menu* m);
	void activate(bool forward);
	void draw(int* xpos, int* ypos, bool selected);
	int player;
};

struct SkillSlot : MenuSlot {
	SkillSlot();
	void activate(bool forward);
	void draw(int* xpos, int* ypos, bool selected);
};

class Menu {
public:
	enum menutype { NOMENU, MAIN, OPTIONS, CONTROLS, INGAME };
	enum menustatus { STARTGAME, EXIT, NONE };

	Menu(menutype t);
	menustatus run(int delta);
	menutype type;
	menustatus status;
	bool readyforkey;

	// for when a new use key is being chosen
	bool choosingkey;	// which player is choosing a use key?
	std::string msgline;
//	CL_InputBuffer buffer;
	int keyidpicked;

private:
	int currententry;
	int maxentries;
	int lasttime;
	MenuSlot* slots[11];
	
	void draw();
	void changeType(menutype t);

	static void toggleFullscreen();
	static void toggleSound();
	static void toggleMusic();
	static void cycleDifficulty(bool forward);
	static void changeKeyGroup(int selected, bool forward);

	static void exitMenu(Menu* m);
	static void quitGame(Menu* m);
	static void start1Game(Menu* m);
	static void start2Game(Menu* m);
	static void optionsMenu(Menu* m);
	static void controlsMenu(Menu* m);

	void saveOptions();
	Menu::menutype prevtype;
};

#endif
