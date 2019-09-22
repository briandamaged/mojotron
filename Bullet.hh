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

#ifndef BULLET_HH
#define BULLET_HH

#include "Thing.hh"
#include "World.hh"

class Player;

extern World* worldobj;

class Bullet : public Thing {

protected:
	Player* owner;

public:
	// needs owner pointer so it can modify playerstats
	Bullet(	Globals::sprindex _spr, int type, int _xpos, int _ypos, 
		int _xdir, int _ydir, Player* _owner=0); 
	virtual ~Bullet() {} 
	
	bool listensEnemy() { return true; }
	bool reflective, orientated, flame;
	
	void move(int delta);
	void startPos();

	int getSpeed();
	int speed;

	void tookEnemy(Thing* x);
	void tookObstacle(Thing* x);

	int range, power;
};

class ThrownBomb : public Bullet {
public:
	ThrownBomb(	Globals::sprindex _spr, int _xpos, int _ypos, 
			int _xdir, int _ydir);
	bool listensEnemy() { return false; }
	void move(int delta);
	int getSpeed();
};

class Spike : public Bullet {
public:
	Spike(	Globals::sprindex _spr, int _xpos, int _ypos, 
		int _xdir, int _ydir);
	int getSpeed();
	bool listensEnemy() 		{ return false; }
	bool listensPlayer() 		{ return true; }
	void tookPlayer(Thing* play) 	{ play->hurt(100, xposdir, yposdir); }
	void recoil(int injury, int xdir, int ydir) { } // recoilling spikes look weird
	Thing::type getType() 		{ return Thing::ENEMY; }
};

class RoundBullet : public Thing {
public:
	// direction fields too
	RoundBullet(	Globals::sprindex _spr, int type, int _xpos, int _ypos, 
			int _xdir, int _ydir); 
	~RoundBullet() {} 
	
	void move(int delta);
	bool listensPlayer() 	{ return true; }
	void startPos() 	{ health = 0; }
	void tookPlayer(Thing* x) { x->hurt(100, xposdir, yposdir); health = 0; }

	/* 	This stuff is here 'cos I want barricades to absorb
		roundbullets and so that roundbullets don't absorb player bullets */
	void hurt(int injury, int xdir, int ydir)	{ }		// unkillable
	void recoil(int injury, int xdir, int ydir)	{ health = 0; }	// get soaked up by barricade
	
	int range, speed;
};

#endif
