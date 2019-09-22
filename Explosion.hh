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

#ifndef EXPLOSION_HH
#define EXPLOSION_HH

#include "Thing.hh"

class Explosion : public Thing {
public:
	static void largeExplosion(Globals::sprindex t, int size, int x, int y);

	Explosion(Globals::sprindex _spr, int _xpos, int _ypos, int xdir, int ydir);
	void move(int delta);
	bool listensObstacle()	{ return false; }
	void startPos()		{ }
};

#endif
