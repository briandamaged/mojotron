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
#include <vector>
#include <memory>

class Menu;

typedef void (*MenuActivateFunc)(bool forward, Menu *m, int player, bool movement);

struct MenuSlot {
	MenuSlot(std::string n, MenuActivateFunc a, int l);
	virtual ~MenuSlot() { }
	virtual void draw(int* xpos, int* ypos, bool selected);
	std::string name;
	MenuActivateFunc activatefunc;
	int linespacing;
	int player;
	bool movement;
};

struct BooleanSlot : MenuSlot {
	BooleanSlot(std::string n, bool* b, MenuActivateFunc a);
	void draw(int* xpos, int* ypos, bool selected);
	bool* state;
};

struct KeysSlot : MenuSlot {
	KeysSlot(int p, bool m);
	void draw(int* xpos, int* ypos, bool selected);

	static void activate(bool forward, Menu *s, int player, bool movement);
};

struct UseKeySlot : MenuSlot {
	UseKeySlot(int p);
	void draw(int* xpos, int* ypos, bool selected);

	static void activate(bool forward, Menu *s, int player, bool movement);
};

struct SkillSlot : MenuSlot {
	SkillSlot(int l);
	void draw(int* xpos, int* ypos, bool selected);

	static void activate(bool forward, Menu *s, int player, bool movement);
};

class Menu {
public:
	enum menutype { NOMENU, MAIN, OPTIONS, CONTROLS, INGAME };
	enum menustatus { STARTGAME, EXIT, NONE };

	Menu(menutype t);
	~Menu();

	menustatus run(int delta);
	bool checkActive();

	menutype type;
	menustatus status;
	bool readyforkey;

	// for when a new use key is being chosen
	bool choosingkey;	// which player is choosing a use key?
	std::string msgline;
	int keyidpicked;

private:
	bool active;
	int currententry;
	int maxentries;
	int lasttime;
	std::vector<std::unique_ptr<MenuSlot>> slots;
	
	void draw();
	void changeType(menutype t);

	static void toggleFullscreen(bool forward, Menu *m, int player, bool movement);
	static void toggleSound(bool forward, Menu *m, int player, bool movement);
	static void toggleMusic(bool forward, Menu *m, int player, bool movement);
	static void cycleDifficulty(bool forward);
	static void changeKeyGroup(int selected, bool forward);

	static void exitMenu(bool forward, Menu* m, int player, bool movement);
	static void quitGame(bool forward, Menu* m, int player, bool movement);
	static void start1Game(bool forward, Menu* m, int player, bool movement);
	static void start2Game(bool forward, Menu* m, int player, bool movement);
	static void optionsMenu(bool forward, Menu* m, int player, bool movement);
	static void controlsMenu(bool forward, Menu* m, int player, bool movement);

	Menu::menutype prevtype;
};

#endif
