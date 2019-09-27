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

#include <stdio.h>
//#include <ClanLib/core.h>
#include "InputState.hh"
#include "Globals.hh"
#include "ConfigFile.hh"

using namespace std;
extern Globals* globals;

bool InputState::anyKey(false);

InputAxis::InputAxis(int minus_k, int plus_k)
{
	minus_key = InputState::convert_clanlib(minus_k);
	plus_key = InputState::convert_clanlib(plus_k);
}

int InputAxis::get_pos()
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[minus_key])
		return -1;
	else if (state[plus_key])
		return 1;
	else
		return 0;
}

InputButton::InputButton(int b)
: button(b)
{
}

int InputButton::is_pressed()
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	return state[button];
}

InputState* InputState::playercontrols[2] = { NULL, NULL };
std::string* InputState::keygroups = NULL;
int InputState::numkeygroups = 0;

void InputState::initControls(Config conf) {
	readGroups();

	for (int i=0; i < 2; i++) {
		playercontrols[i] = new InputState(i, conf.movegroup[i],
					conf.aimgroup[i], conf.usekey[i]);
	}
}

void InputState::readGroups() {
	numkeygroups = globals->loadInt("Controls/numkeygroups");
	keygroups = new std::string[numkeygroups];
	std::string groups_str = globals->loadString("Controls/keygroups");
	const char* groups = groups_str.c_str();

	// interpret keygroup list. man strtok says I shouldn't use strtok(). :(
	int i = 0, j = 0;
	while (groups[i] && j < numkeygroups) {
		keygroups[j] = std::string();	// these are on the stack? uhoh
		while (groups[i] != ',') {
			if (!groups[i])	return;

			keygroups[j] += groups[i];
			i++;
		}
		i++;	j++;
	}
}

void InputState::deinitControls() {
	for (int i=0; i < 2; i++) {
		delete playercontrols[i];
	}
}

InputState::InputState(int _playernumber, string movegroup, 
			string aimgroup, int usek) {
	playernumber = _playernumber;
	movex = movey = firex = firey = NULL;
	use = NULL;
	keyboardnumber = 0;

	if (globals->verbosity > 0)
		cout << "Assigning keys to Input groups" << std::endl;

	// read the keys
	setUseKey(convert_clanlib(usek));
	setupKeyAxisPair(getKeygroupNumber(movegroup), true);
	setupKeyAxisPair(getKeygroupNumber(aimgroup), false);
	
	// detect joysticks
/*	unsigned int joy = playernumber*2;	// players get two joysticks each
	if (CL_Input::joysticks.size() > joy) {
		movex->add(CL_Input::joysticks[joy]->get_axis(0));
		movey->add(CL_Input::joysticks[joy]->get_axis(1));
		joy++;

		if (CL_Input::joysticks.size() > joy) {
			firex->add(CL_Input::joysticks[joy]->get_axis(0));
			firey->add(CL_Input::joysticks[joy]->get_axis(1));
		}
	}*/

	if (globals->verbosity > 0)
		cout << "All assigned." << endl;
}

void InputState::setUseKey(int k) {
	if (use)	delete use;

	usekey = k;

/*	if (k > CL_NUM_KEYS || k == CL_NO_KEY || k == CL_KEY_NONE_OF_THE_ABOVE) {
		cout << "Bad use key set, using SPACE instead." << endl;
		k = CL_KEY_SPACE;
	}*/
	use = new InputButton(k);
}

std::string InputState::getUseKey() {
/*	switch (usekey) {
		// special cases, whitespace
		case CL_KEY_SPACE:	return "space";
		case CL_KEY_ENTER:	return "return";
		case CL_KEY_KP_ENTER:	return "enter";
		case CL_KEY_LCTRL:
		case CL_KEY_RCTRL:	return "ctrl";
		case CL_KEY_ALT:	return "alt";
		case CL_KEY_LSHIFT:	
		case CL_KEY_RSHIFT:	return "shift";
		case CL_KEY_TAB:	return "tab";
		case CL_KEY_NONE_OF_THE_ABOVE: return "Invalid key";
		default: {
			// FIXME: I am a hack
			char c[2];
			c[1] ='\0';
			if (usekey >= CL_KEY_A && usekey <= CL_KEY_Z) {
				c[0] = (usekey - CL_KEY_A) + 'a';
			} else if (usekey >= CL_KEY_0 && usekey <= CL_KEY_9) {
				c[0] = (usekey - CL_KEY_0) + '0';
			} else if (usekey >= CL_KEY_KP_0 && usekey <= CL_KEY_KP_9) {
				// it gives me none of the above instead. never reached
				c[0] = (usekey - CL_KEY_KP_0) + '0';
			} else {
				return "Some key";
			}
			return std::string(c);
		}
	}*/
	return "";
}

void InputState::setupKeyAxisPair(int keygroup, bool ismovegroup) {
//	if (keygroup >= numkeygroups || keygroup < 0)
//		throw CL_Error("Keygroup out of range.");
	std::string name = "Controls/" + keygroups[keygroup];


	// Normal direction keys
	InputAxis *x = new InputAxis(globals->loadInt(name + "/left"), globals->loadInt(name + "/right"));
	InputAxis *y = new InputAxis(globals->loadInt(name + "/up"), globals->loadInt(name + "/down"));
	if (ismovegroup) {
		//x = movex;	y = movey;
		/* Memory leak?
		 * Will the axes stored inside get freed?
		 */
		if (movex)
		{
			delete movex;	delete movey;
		}
		movex = x;	movey = y;
		movekeygroup = keygroup;
		movekeys = globals->loadString(name + "/keys").c_str();
	} else {
		//x = firex;	y = firey;
		if (firex)
		{
			delete firex;	delete firey;
		}
		firex = x;	firey = y;
		firekeygroup = keygroup;
		firekeys = globals->loadString(name + "/keys").c_str();
	}

	// Diagonal movement key numbers
/*	int uprighti, uplefti, downrighti, downlefti;
	uprighti = uplefti = downrighti = downlefti = 0;

	uprighti = globals->loadInt(name + "/upright");
	uplefti = globals->loadInt(name + "/upleft");
	downrighti = globals->loadInt(name + "/downright");
	downlefti = globals->loadInt(name + "/downleft");

	// if diagonals configured
	if (uprighti && uplefti && downrighti && downlefti) {
		CL_InputButton *upright, *upleft, *downright, *downleft;

		upright = CL_Input::keyboards[i]->get_button(uprighti);
		upleft = CL_Input::keyboards[i]->get_button(uplefti);
		downright = CL_Input::keyboards[i]->get_button(downrighti);
		downleft = CL_Input::keyboards[i]->get_button(downlefti);

		x->add(new CL_InputButtonToAxis_Digital(upleft, upright));
		x->add(new CL_InputButtonToAxis_Digital(downleft, downright));
		y->add(new CL_InputButtonToAxis_Digital(upleft, downleft));
		y->add(new CL_InputButtonToAxis_Digital(upright, downright));
		// FIXME: I hope the axis groups will delete these
		//delete upright;
		//delete upleft;
		//delete downright;
		//delete downleft;
	}*/
}

std::string InputState::serialiseKeys() {
	int xmov = 1 + (int)movex->get_pos();
	int ymov = 1 + (int)movey->get_pos();
	int xaim = 1 + (int)firex->get_pos();
	int yaim = 1 + (int)firey->get_pos();
	int iuse = use->is_pressed();

	char buffer[12];
	sprintf(buffer, "Keys:%1d%1d%1d%1d%d ",
		xmov, ymov, xaim, yaim, iuse);
	return std::string(buffer);
}

char InputState::getKeyChar(	bool ismoveaxispair,
				int axis, int dir) {
	int direction = NONE;
	if (axis)	direction += 3*dir;	// vertical
	else		direction += dir;	// horizontal

	//return keys[(ismoveaxispair * NUMDIR) + direction];
	if (ismoveaxispair)	return movekeys[direction];
	else			return firekeys[direction];
}

int InputState::getKeyAxisPair(bool ismoveaxispair) {
	if (ismoveaxispair) {
		return movekeygroup;
	} else {
		return firekeygroup;
	}
}

int InputState::getKeygroupNumber(std::string name) {
	// linear search
	for (int i=0; i < numkeygroups; i++) {
		if (keygroups[i] == name) {
			return i;
		}
	}
			SDL_Quit();
			exit(0);
//	throw CL_Error("No defined keygroup named " + name);
}

InputState::~InputState() {
	delete movex;
	delete movey;
	delete firex;
	delete firey;
	delete use;
}

void InputState::process()
{
	SDL_Event sdlevent;
	int avail = 0;
	for (avail = SDL_PollEvent(&sdlevent); avail; avail = SDL_PollEvent(&sdlevent))
	{
		if ((sdlevent.type == SDL_QUIT)/* || (_p_common_resources->key.num1.get() && _p_common_resources->key.num2.get())*/)
		{
//			Mix_Quit();
			SDL_Quit();
			exit(0);
		}
		else if (sdlevent.type == SDL_KEYDOWN)
		{
			anyKey = true;
		}
	}
}

bool InputState::anyKeyPress()
{
	bool ans = anyKey;
	anyKey = false;
	return ans;
}

int InputState::convert_clanlib(int k)
{
	switch (k)
	{
		case 23:
			return SDL_SCANCODE_W;
			break;
		case 1:
			return SDL_SCANCODE_A;
			break;
		case 19:
			return SDL_SCANCODE_S;
			break;
		case 4:
			return SDL_SCANCODE_D;
			break;
		case 52:
			return SDL_SCANCODE_UP;
			break;
		case 50:
			return SDL_SCANCODE_LEFT;
			break;
		case 53:
			return SDL_SCANCODE_DOWN;
			break;
		case 51:
			return SDL_SCANCODE_RIGHT;
			break;
		case 63:
			return SDL_SCANCODE_SPACE;
			break;
		case 79:
			return SDL_SCANCODE_KP_ENTER;
			break;
		case 9:
			return SDL_SCANCODE_I;
			break;
		case 10:
			return SDL_SCANCODE_J;
			break;
		case 11:
			return SDL_SCANCODE_K;
			break;
		case 12:
			return SDL_SCANCODE_L;
			break;
		case 83:
			return SDL_SCANCODE_KP_2;
			break;
		case 85:
			return SDL_SCANCODE_KP_4;
			break;
		case 87:
			return SDL_SCANCODE_KP_6;
			break;
		case 89:
			return SDL_SCANCODE_KP_8;
			break;
		default:
			break;
	}
	return SDL_SCANCODE_O;
}
