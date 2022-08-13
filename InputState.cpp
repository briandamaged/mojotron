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
#include "Application.h"
#include "InputState.hh"
#include "Intro.hh"
#include "Globals.hh"
#include "ConfigFile.hh"

using namespace std;
extern Globals* globals;

bool InputState::anyKey(false);
int InputState::lastScancode(0);

AbstractAxisPair::~AbstractAxisPair() {
}

InputAxisPair::InputAxisPair(int minus_xk, int plus_xk, int minus_yk, int plus_yk)
: joy(NULL)
{
	minus_xkey = InputState::convert_clanlib(minus_xk);
	plus_xkey = InputState::convert_clanlib(plus_xk);
	minus_ykey = InputState::convert_clanlib(minus_yk);
	plus_ykey = InputState::convert_clanlib(plus_yk);
}

void InputAxisPair::get_pos(int &x, int &y)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	x = 0;
	y = 0;
	if (joy) {
		int xval = SDL_GameControllerGetAxis(joy, (SDL_GameControllerAxis)xaxis);
		int yval = SDL_GameControllerGetAxis(joy, (SDL_GameControllerAxis)yaxis);
		int angle = (int(atan2(yval, xval) * 180 / M_PI) + 360) % 360;
		if ((angle <= 60 || angle >= 300) && (xval > 3200))
			x = 1;
		else if ((angle >= 120 && angle <= 240) && (xval < -3200))
			x = -1;
		if ((angle >= 30 && angle <= 150) && (yval > 3200))
			y = 1;
		else if ((angle >= 210 && angle <= 330) && (yval < -3200))
			y = -1;
	}
	if (state[minus_xkey])
		x = -1;
	else if (state[plus_xkey])
		x = 1;
	if (state[minus_ykey])
		y = -1;
	else if (state[plus_ykey])
		y = 1;
}

void InputAxisPair::set_controller(SDL_GameController *j, int xa, int ya)
{
	joy = j;
	xaxis = xa;
	yaxis = ya;
}

int DemoAxisPair::moveinput[DEMOMOVEINPUT][3] = {{1500, 1, 0}, {2500, 1, 1}, {7000, -1, 0}, {9500, 0, -1}, {15000, 1, 0}, {500000, 0, 1}};
int DemoAxisPair::fireinput[DEMOFIREINPUT][3] = {{900, 1, 0}, {1000, 1, 1}, {1100, 0, 1}, {1900, 1, 1}, {2000, 0, 1}, {2100, -1, 1}, {4000, -1, 0}, {12000, 1, 0}, {500000, -1, 0}};

DemoAxisPair::DemoAxisPair(bool m) : move(m) {
}

void DemoAxisPair::get_pos(int &x, int &y) {
	if (move) {
		for (int i = 0; i < DEMOMOVEINPUT; i++) {
			if (Intro::demotime <= moveinput[i][0]) {
				x = moveinput[i][1];
				y = moveinput[i][2];
				return;
			}
		}
	}
	else {
		for (int i = 0; i < DEMOFIREINPUT; i++) {
			if (Intro::demotime <= fireinput[i][0]) {
				x = fireinput[i][1];
				y = fireinput[i][2];
				return;
			}
		}
	}
	x = 0;
	y = 0;
}

AbstractButton::~AbstractButton() {
}

InputButton::InputButton(int b)
: button(b), joy(NULL)
{
}

int InputButton::is_pressed()
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	int ans = state[button];
	if ((ans == 0) && (joy)) {
		int val = SDL_GameControllerGetAxis(joy, (SDL_GameControllerAxis)joy_button);
		if (val > 3200)
			ans = 1;
	}
	return ans;
}

void InputButton::set_controller(SDL_GameController *j, int b)
{
	joy = j;
	joy_button = b;
}

int DemoButton::is_pressed() {
	return 0;
}

InputState* InputState::playercontrols[2] = { NULL, NULL };
InputState* InputState::democontrols = NULL;
std::string* InputState::keygroups = NULL;
int InputState::numkeygroups = 0;

void InputState::initControls(Config conf) {
	readGroups();

	for (int i=0; i < 2; i++) {
		playercontrols[i] = new InputState(i, conf.movegroup[i],
					conf.aimgroup[i], conf.usekey[i]);
	}
	democontrols = new InputState();
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
	move = fire = NULL;
	use = NULL;
	joystick = NULL;

	if (globals->verbosity > 0)
		cout << "Assigning keys to Input groups" << std::endl;

	// read the keys
	setUseKey(convert_clanlib(usek));
	setupKeyAxisPair(getKeygroupNumber(movegroup), true);
	setupKeyAxisPair(getKeygroupNumber(aimgroup), false);
	
	// detect joysticks
	int joy = SDL_NumJoysticks();
	int found = 0;
	for(int JoystickIndex=0; JoystickIndex < joy; ++JoystickIndex)
	{
		if (!SDL_IsGameController(JoystickIndex))
		{
			continue;
		}
		if (found == playernumber) {
			joystick = SDL_GameControllerOpen(JoystickIndex);
			dynamic_cast<InputAxisPair*>(move)->set_controller(joystick, SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY);
			dynamic_cast<InputAxisPair*>(fire)->set_controller(joystick, SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY);
			dynamic_cast<InputButton*>(use)->set_controller(joystick, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
			break;
		}
		else
			found++;
	}

	if (globals->verbosity > 0)
		cout << "All assigned." << endl;
}

InputState::InputState() {
	playernumber = 0;
	move = new DemoAxisPair(true);
	fire = new DemoAxisPair(false);
	use = new DemoButton();
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
	switch (usekey) {
		// special cases, whitespace
		case SDL_SCANCODE_SPACE:	return "space";
		case SDL_SCANCODE_RETURN:	return "return";
		case SDL_SCANCODE_KP_ENTER:	return "enter";
		case SDL_SCANCODE_LCTRL:
		case SDL_SCANCODE_RCTRL:	return "ctrl";
		case SDL_SCANCODE_LALT:
		case SDL_SCANCODE_RALT:	return "alt";
		case SDL_SCANCODE_LSHIFT:	
		case SDL_SCANCODE_RSHIFT:	return "shift";
		case SDL_SCANCODE_TAB:	return "tab";
		case SDL_SCANCODE_LEFTBRACKET:	return "[";
		default: {
			// FIXME: I am a hack
			char c[2];
			c[1] ='\0';
			if (usekey >= SDL_SCANCODE_A && usekey <= SDL_SCANCODE_Z) {
				c[0] = (usekey - SDL_SCANCODE_A) + 'a';
			} else if (usekey >= SDL_SCANCODE_0 && usekey <= SDL_SCANCODE_9) {
				c[0] = (usekey - SDL_SCANCODE_0) + '0';
			} else if (usekey >= SDL_SCANCODE_KP_0 && usekey <= SDL_SCANCODE_KP_9) {
				// it gives me none of the above instead. never reached
				c[0] = (usekey - SDL_SCANCODE_KP_0) + '0';
			} else {
				return "Some key";
			}
			return std::string(c);
		}
	}
	return "";
}

void InputState::setupKeyAxisPair(int keygroup, bool ismovegroup) {
//	if (keygroup >= numkeygroups || keygroup < 0)
//		throw CL_Error("Keygroup out of range.");
	std::string name = "Controls/" + keygroups[keygroup];


	// Normal direction keys
	InputAxisPair *a = new InputAxisPair(globals->loadInt(name + "/left"), globals->loadInt(name + "/right"), globals->loadInt(name + "/up"), globals->loadInt(name + "/down"));
	if (ismovegroup) {
		/* Memory leak?
		 * Will the axes stored inside get freed?
		 */
		if (move)
		{
			delete move;
		}
		move = a;
		movekeygroup = keygroup;
		movekeys = globals->loadString(name + "/keys");
	} else {
		if (fire)
		{
			delete fire;
		}
		fire = a;
		firekeygroup = keygroup;
		firekeys = globals->loadString(name + "/keys");
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
	int xmov, ymov, xaim, yaim;
	move->get_pos(xmov, ymov);
	xmov++; ymov++;
	fire->get_pos(xaim, yaim);
	xaim++; yaim++;
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
	Application::getApplication()->quit();
	return 0;
//	throw CL_Error("No defined keygroup named " + name);
}

InputState::~InputState() {
	delete move;
	delete fire;
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
			Application::getApplication()->quit();
		}
		else if (sdlevent.type == SDL_KEYDOWN)
		{
			anyKey = true;
			lastScancode = sdlevent.key.keysym.scancode;
		}
	}
}

bool InputState::anyKeyPress()
{
	bool ans = anyKey;
	anyKey = false;
	return ans;
}

int InputState::getLastScancode()
{
	return lastScancode;
}

int InputState::convert_clanlib(int k)
{
	switch (k)
	{
		case 102:
			return SDL_SCANCODE_LEFTBRACKET;
			break;
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
		case 5:
			return SDL_SCANCODE_E;
			break;
		case 6:
			return SDL_SCANCODE_F;
			break;
		case 7:
			return SDL_SCANCODE_G;
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
		case 18:
			return SDL_SCANCODE_R;
			break;
		case 24:
			return SDL_SCANCODE_X;
			break;
		case 26:
			return SDL_SCANCODE_Z;
			break;
		case 56:
			return SDL_SCANCODE_LSHIFT;
			break;
		case 58:
			return SDL_SCANCODE_LALT;
			break;
		case 103:
			return SDL_SCANCODE_RIGHTBRACKET;
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

int InputState::convert_SDL2(int k)
{
	switch (k)
	{
		case SDL_SCANCODE_LEFTBRACKET:
			return 102;
			break;
		case SDL_SCANCODE_W:
			return 23;
			break;
		case SDL_SCANCODE_A:
			return 1;
			break;
		case SDL_SCANCODE_S:
			return 19;
			break;
		case SDL_SCANCODE_D:
			return 4;
			break;
		case SDL_SCANCODE_UP:
			return 52;
			break;
		case SDL_SCANCODE_LEFT:
			return 50;
			break;
		case SDL_SCANCODE_DOWN:
			return 53;
			break;
		case SDL_SCANCODE_RIGHT:
			return 51;
			break;
		case SDL_SCANCODE_SPACE:
			return 63;
			break;
		case SDL_SCANCODE_KP_ENTER:
			return 79;
			break;
		case SDL_SCANCODE_E:
			return 5;
			break;
		case SDL_SCANCODE_F:
			return 6;
			break;
		case SDL_SCANCODE_G:
			return 7;
			break;
		case SDL_SCANCODE_I:
			return 9;
			break;
		case SDL_SCANCODE_J:
			return 10;
			break;
		case SDL_SCANCODE_K:
			return 11;
			break;
		case SDL_SCANCODE_L:
			return 12;
			break;
		case SDL_SCANCODE_R:
			return 18;
			break;
		case SDL_SCANCODE_X:
			return 24;
			break;
		case SDL_SCANCODE_Z:
			return 26;
			break;
		case SDL_SCANCODE_LSHIFT:
			return 56;
			break;
		case SDL_SCANCODE_LALT:
			return 58;
			break;
		case SDL_SCANCODE_RIGHTBRACKET:
			return 103;
			break;
		case SDL_SCANCODE_KP_2:
			return 83;
			break;
		case SDL_SCANCODE_KP_4:
			return 85;
			break;
		case SDL_SCANCODE_KP_6:
			return 87;
			break;
		case SDL_SCANCODE_KP_8:
			return 89;
			break;
		default:
			break;
	}
	return SDL_SCANCODE_O;
}
