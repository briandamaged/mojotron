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

#include <ClanLib/display.h>

struct Config;

class InputState {
public:
	// should make max players a define somewhere
	static InputState* playercontrols[2];
	static std::string* keygroups;
	static int numkeygroups;
	static void initControls(Config c);
	static void deinitControls();
	static void readGroups();

	CL_InputAxis_Group* movex;
	CL_InputAxis_Group* movey;
	CL_InputAxis_Group* firex;
	CL_InputAxis_Group* firey;
	CL_InputButton_Group* use;

	enum direction {	UPLEFT, UP, UPRIGHT,
				LEFT, NONE, RIGHT,
				DOWNLEFT, DOWN, DOWNRIGHT, NUMDIR };
	const char* movekeys;
	const char* firekeys;
	int movekeygroup;
	int firekeygroup;
	int usekey;

	// going to assume all of a players keys are on one keyboard
	int keyboardnumber;
	int playernumber;

	// direction key -> char. NB: 0 is x axis, 1 is y axis
	char getKeyChar(bool ismoveaxispair, int axis, int dir);
	int getKeyAxisPair(bool ismoveaxispair);
	void setupKeyAxisPair(int keygroupnumber, bool ismovegroup);

	std::string getUseKey();
	void setUseKey(int k);

	std::string serialiseKeys();

private:
	InputState(	int _playernumber, string movegroup,
			string aimgroup, int use);
	~InputState();

	int getKeygroupNumber(std::string name);
};

#endif
