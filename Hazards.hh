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

#ifndef HAZARDS_HH
#define HAZARDS_HH

#include "Thing.hh"

class Obstacle : public Thing {
public:
	Obstacle(Globals::sprindex _spr);
	~Obstacle();
	bool listensPlayer()	{ return true; }
	void move(int delta);
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }
	int column, row;
};

class Grunt : public Thing {

public:
	Grunt() {}
	Grunt(Globals::sprindex _spr, int _speed);
	~Grunt() 		{ bonusGen(0, xpos, ypos); explode(0, Globals::GOREEXPLODE); }
	
	void move(int delta);
	void hurt(int injury, int xdir=0, int ydir=0);
	
	bool listensPlayer() 	{ return true; }
	void tookPlayer(Thing* play);
	Thing::type getType()	{ return Thing::ENEMY; }
	int getSpeed();
	static void calcSpeed(int delta, int number);

	static int agespeedup, numspeedup;
	static int age;
	static bool escapepod, randompod, bouncingpod;
};

class Ball : public Thing {
public:
	Ball();
	Ball(Globals::sprindex _spr);
	~Ball()			{ bonusGen(1, xpos, ypos); explode(0, Globals::METALLICEXPLODE); }
	
	void move(int delta);
	Thing::type getType()	{ return Thing::ENEMY; }
	bool listensPlayer()	{ return true; }
	bool listensEnemy()	{ return true; }
	void tookEnemy(Thing* x);
	void tookPlayer(Thing* x);
	void recoil(int injury, int xdir, int ydir);
};

class Spiker : public Thing {
public:
	Spiker(Globals::sprindex _spr);
	~Spiker()		{ bonusGen(2, xpos, ypos); explode(0, Globals::GOREEXPLODE); }
	
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }

	void move(int delta);
};

class Asteroid : public Thing {
protected:
	int animframe, size, recoiltimer;
	void firstSplit();
	void secondSplit();

public:
	Asteroid();
	Asteroid(int _xpos, int _ypos, int _size, int _xdir=0, int _wobblestart=0);
	~Asteroid()		{ bonusGen(0, xpos, ypos); explode(0, Globals::GOREEXPLODE); }
	
	void move(int delta);
	Thing::type getType()	{ return Thing::ENEMY; }
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* x) { x->hurt(100); }
};

class Sniper : public Thing {
public:
	Sniper(Globals::sprindex _spr);
	~Sniper()		{ bonusGen(4, xpos, ypos); explode(0, Globals::GOREEXPLODE); }
	
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }

	void move(int delta);

	int remainder, remaindermax;
};

class RightAngler : public Thing {
public:
	RightAngler(Globals::sprindex _spr, bool _verthunting);
	~RightAngler()		{ bonusGen(2, xpos, ypos); explode(0, Globals::GOREEXPLODE); }

	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	void tookObstacle(Thing* obst);
	Thing::type getType()	{ return Thing::ENEMY; }

	void move(int delta);
	void startPos();

	bool goingup, attacking, verthunting;
	int huntspeed, attackspeed;
	int orientation;
};

class SimpleBoss : public Thing {
public:
	SimpleBoss(Globals::sprindex _spr, int _number);
	~SimpleBoss()		{ bonusGen(10, xpos, ypos); explode(6, Globals::FLAMEEXPLODE); }

	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	void tookObstacle(Thing* obst);
	Thing::type getType()	{ return Thing::ENEMY; }

	void move(int delta);
	void startPos();
	void hurt(int injury, int xdir, int ydir);

	int remainder, reloadtime, number;
};

// add changeTarget()?
class Drone : public Thing {
public:
	Drone(Globals::sprindex _spr, Thing* _target=0, bool soakbullets=true);
	~Drone();

	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }
	void startPos();

	void move(int delta);
	Thing* target;
	int timebetweenlook;
};

class RockMan : public Thing {
public:
	RockMan(Globals::sprindex _spr);
	~RockMan()		{ }

	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }

	void move(int delta);
	void hurt(int injury, int _xdir, int _ydir);
};

class Turret : public Thing {
public:
	Turret(Globals::sprindex _spr, int _fansize);
	~Turret()		{ bonusGen(2, xpos, ypos); explode(3, Globals::METALLICEXPLODE); }
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }
	void move(int delta);
	int fansize, reloadtime, bulletspeed;
};

class Soldier : public Thing {
public:
	Soldier(Globals::sprindex _spr);
	~Soldier()		{ bonusGen(1, xpos, ypos); explode(0, Globals::FLAMEEXPLODE); }
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }
	void move(int delta);
	int timebetweenlook, bulletspeed;
	static bool playedfire;
};

class CircleBoss : public Thing {
public:
	CircleBoss(Globals::sprindex _spr, int x, int y, int _speed, float _start);
	~CircleBoss()		{ bonusGen(9, xpos, ypos); explode(6, Globals::FLAMEEXPLODE); }
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	void tookObstacle(Thing* obst);
	Thing::type getType()	{ return Thing::ENEMY; }
	void move(int delta);
	void startPos();
	void hurt(int injury, int xdir, int ydir);
	int spawnspeed;
	int origxwobblecent, origywobblecent;
	float start;
};

class Centipede : public Thing {
public:
	Centipede(Globals::sprindex _spr, Thing* attachedto=0);
	~Centipede();
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }
	void move(int delta);
	void startPos();
	void getInBehind();	// ...I'm from New Zealand
	void hurt(int injury, int xdir=0, int ydir=0);

	// used in conglomerate
	Thing* boss;

private:
	int turningcirc, seekaccuracy;
	int feartimer, maxfeartimer;
};

class Cellphone : public Thing {
public:
	Cellphone(Globals::sprindex _spr, Thing* attachedto=0);
	~Cellphone();
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }
	void move(int delta);
	int timebetweenlook, dronespeed;
};

class Spiral : public Thing {
public:
	Spiral(Globals::sprindex _spr);
	~Spiral()		{ bonusGen(2, xpos, ypos); explode(0, Globals::GOREEXPLODE); }
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }
	void move(int delta);
	int starttime, expandstep;
};

class Laser : public Thing {
	public:
	Laser(Globals::sprindex _spr, bool _vertical);
	~Laser()		{ }
	bool listensPlayer()	{ return true; }
	void tookPlayer(Thing* play)
				{ play->hurt(100); }
	Thing::type getType()	{ return Thing::ENEMY; }
	void move(int delta);
	void startPos();
	bool colcheck(Thing* thingb);
	void draw();
	void hurt(int injury, int _xdir, int _ydir);
	bool vertical;
	int firetime, firingperiod, reloadperiod, brightness;
};

#endif
