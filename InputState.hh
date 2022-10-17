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

#ifndef INPUTSTATE_HH
#define INPUTSTATE_HH

#include <SDL.h>
#include <string>
#include <iostream>
using namespace std;

struct Config;

class AbstractAxisPair {
public:
	virtual ~AbstractAxisPair();

	virtual void get_pos(int &x, int &y) = 0;
};

class InputAxisPair : public AbstractAxisPair
{
public:
	InputAxisPair(int minus_xk, int plus_xk, int minus_yk, int plus_yk);

	void get_pos(int &x, int &y);
	void set_controller(SDL_GameController *j, int xa, int ya);

private:
	int minus_xkey, plus_xkey;
	int minus_ykey, plus_ykey;
	int xaxis, yaxis;
	SDL_GameController *joy;
};

#define DEMOMOVEINPUT 6
#define DEMOFIREINPUT 9

class DemoAxisPair : public AbstractAxisPair {
public:
	DemoAxisPair(bool m);

	void get_pos(int &x, int &y);

private:
	bool move;
	int xprev, yprev;

	static int moveinput[DEMOMOVEINPUT][3];
	static int fireinput[DEMOFIREINPUT][3];
};

class AbstractButton {
public:
	virtual ~AbstractButton();

	virtual int is_pressed() = 0;
};

class InputButton : public AbstractButton
{
public:
	InputButton(int b);

	int is_pressed();
	void set_controller(SDL_GameController *j, int b);

private:
	int button;
	int joy_button;
	SDL_GameController *joy;
};

class DemoButton : public AbstractButton {
public:
	int is_pressed();
};

class InputState {
public:
	// should make max players a define somewhere
	static InputState* playercontrols[2];
	static InputState* democontrols;
	static std::string* keygroups;
	static int numkeygroups;
	static void initControls(const Config &c);
	static void deinitControls();
	static void readGroups();

	AbstractAxisPair* move;
	AbstractAxisPair* fire;
	AbstractButton* use;
	SDL_GameController *joystick;

	enum direction {	UPLEFT, UP, UPRIGHT,
				LEFT, NONE, RIGHT,
				DOWNLEFT, DOWN, DOWNRIGHT, NUMDIR };
	std::string movekeys;
	std::string firekeys;
	int movekeygroup;
	int firekeygroup;
	int usekey;

	int playernumber;

	// direction key -> char. NB: 0 is x axis, 1 is y axis
	char getKeyChar(bool ismoveaxispair, int axis, int dir);
	int getKeyAxisPair(bool ismoveaxispair);
	void setupKeyAxisPair(int keygroupnumber, bool ismovegroup);

	std::string getUseKey();
	void setUseKey(int k);

	std::string serialiseKeys();

	static void process();
	static bool anyKeyPress();
	static int getLastScancode();
	static int convert_clanlib(int k);
	static int convert_SDL2(int k);

private:
	InputState(	int _playernumber, string movegroup,
			string aimgroup, int use);
	InputState();
	~InputState();

	int getKeygroupNumber(std::string name);

	static bool anyKey;
	static int lastScancode;
};

#endif
