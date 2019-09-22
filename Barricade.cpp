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

#include "Barricade.hh"
#include "Sprite.hh"

using namespace std;
extern Globals* globals;

Barricade::Barricade() : Thing(Globals::CRATE) {
	init();
}

Barricade::Barricade(Globals::sprindex _spr, int _xpos, int _ypos) : Thing(_spr) {
	init();
	xpos = _xpos; ypos = _ypos;
}

void Barricade::init() {
	health = globals->loadInt("Constants/barricadehealth");
	speed = globals->loadInt("Constants/barricadespeed");
	frictionconst = globals->loadInt("Constants/barricadefriction");
	killsplayerbullet = optionalkill = true;
}

void Barricade::move(int delta) {
	if (xposdir || yposdir) {
		xpos += (speed * delta * xposdir >> 8);
		ypos += (speed * delta * yposdir >> 8);
	}

	int friction = delta * frictionconst;
	if (xposdir < 0) {
		xposdir += friction;
		if (xposdir > 0)	xposdir = 0;
	} else if (xposdir > 0)	{
		xposdir -= friction;
		if (xposdir < 0)	xposdir = 0;
	}
	if (yposdir < 0) {
		yposdir += friction;
		if (yposdir > 0)	yposdir = 0;
	} else if (yposdir > 0)	{
		yposdir -= friction;
		if (yposdir < 0)	yposdir = 0;
	}

	Thing::move(delta);
}

void Barricade::recoil(int force, int xdir, int ydir) {
	xposdir = xdir * (force/8);
	yposdir = ydir * (force/8);
}

void Barricade::tookPlayer(Thing* x) {
	tookObstacle(x);
}

// don't respawn in a different place
void Barricade::startPos() {}

bool Barricade::listensObstacle() {
	// only listen for the others if you're moving
	if (xposdir == 0 && yposdir == 0)	return false;
	else					return true;
}

BarricadeImpact::BarricadeImpact(int _x, int _y) : 
		Thing(Globals::CRATE) // graphic unused
{
	xpos = _x; ypos = _y;
	optionalkill = true;
}

void BarricadeImpact::tookEnemy(Thing* enemy) {
	enemy->health -= 300;
}

void BarricadeImpact::move(int delta) {
	health -= 50; // survive for one frame
}
