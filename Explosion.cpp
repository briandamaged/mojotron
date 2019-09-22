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

#include "Sprite.hh"
#include "Explosion.hh"
#include "World.hh"

using namespace std;
extern World* worldobj;
extern Globals* globals;

void Explosion::largeExplosion(Globals::sprindex s, int size, int x, int y) {
	Thing *e = new Explosion(s, x, y, 0, 0);
	worldobj->addThing(e);

	for (int ii = 0; ii < size; ii++) {
		int xd = (512 * (ii % 2)) - 256;
		int yd = 256 * (ii / 2) - 256;
		Thing *e = new Explosion(s, x, y, xd, yd);
		worldobj->addThing(e);
		e->jump(globals->loadInt("Constants/explodeheight"));
	}
}

Explosion::Explosion(	Globals::sprindex _spr, int _xpos, int _ypos,
			int xdir, int ydir) : Thing(_spr) {
	speed = globals->loadInt("Constants/explodespreadspeed");
	optionalkill = true;
	xpos = _xpos;	ypos = _ypos;
	xposdir = xdir;	yposdir = ydir;
	frame = 0;
}

void Explosion::move(int delta) {
	xpos += (speed * delta * xposdir) >> 8;
	ypos += (speed * delta * yposdir) >> 8;

	movez(delta);

	frameremainder += delta;
	if (frameremainder > timeperframe) {
		frameremainder -= timeperframe;

		// advance frame
		if (frame < spr->getFrames() - 1)
			frame++;
		else
			// vanish
			health = 0;
	}
}

