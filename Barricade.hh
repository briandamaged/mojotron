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

#ifndef BARRICADE_HH
#define BARRICADE_HH

#include "Thing.hh"

class Barricade : public Thing {
public:
	Barricade();
	Barricade(Globals::sprindex _spr, int _xpos, int _ypos);
	~Barricade()		{ }
	
	void move(int delta);
	Thing::type getType()	{ return Thing::OBSTACLE; }
	bool listensPlayer()	{ return true; }
	bool listensObstacle();
	void recoil(int force, int xdir, int ydir);
	void tookPlayer(Thing* x);
	void startPos();

private:
	void init();
	int frictionconst;
};

/* I'm an invisible thing that hurts nearby enemies and vanishes immediately.
 * This is used to kill monsters underneath new crates.
 * See Bonus.cpp, Instacrate. */
class BarricadeImpact : public Thing {
public:
	BarricadeImpact(int _x, int _y);
	Thing::type getType()	{ return Thing::NONE; }
	bool listensEnemy()	{ return true; }
	void tookEnemy(Thing* enemy);
	void move(int delta);
	void startPos() { health = 0; }
	void draw() {  }	// invisible
};

#endif
