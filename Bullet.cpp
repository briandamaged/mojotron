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

#include "Bullet.hh"
#include "Sprite.hh"
#include "Player.hh"

using namespace std;
extern World* worldobj;
extern Globals* globals;

Bullet::Bullet(	Globals::sprindex _spr, int type, int _xpos, int _ypos, 
		int _xdir, int _ydir, Player* _owner) : Thing(_spr) 
{
	optionalkill = true;
	xpos = _xpos;	ypos = _ypos;
	xposdir = _xdir;	yposdir = _ydir;
	owner = _owner;
	
	if (type & 1) reflective = true; else reflective = false;
	if (type & 2) {
		flame = true;
		power = globals->loadInt("Constants/flamepower");
		range = globals->loadInt("Constants/flamerange");
		speed = globals->loadInt("Constants/flamespeed");
	} else { 
		flame = false;
		power = globals->loadInt("PlayerSpecs/firepower");
		range = globals->loadInt("PlayerSpecs/bullettimeout");
		speed = globals->loadInt("PlayerSpecs/bulletspeed");
	}
	if (type & 4) {
		orientated = true;
		/*	0 1 2
			3 4 5
			6 7 8	*/
		frame = 3*(yposdir>>8) + (xposdir>>8) + 4;
	} else {
		orientated = false;
	}
	if (type & 8) {
		power *= 2;
	}

}

void Bullet::move(int delta) {
	if (!orientated) Thing::move(delta);
	
	// if we're not a mine, count down to timeout
	if (xposdir || yposdir) {
		if (globals->verbosity > 1)
			cout << "Bullet: " << this << " range = " << range << ", health = " << health << endl;
		range -= delta;
		if (range < 0)
			health = 0;
	}

	int displacement = delta * getSpeed();
	xpos += xposdir * displacement >> 8;
	ypos += yposdir * displacement >> 8;

	if (reflective) {
		bounce();
		if (orientated)
			frame = 3*(yposdir>>8) + (xposdir>>8) + 4; // bouncing bullets need to change frame

	} else if (XoutofArena(false) || YoutofArena(false)) {
		if (owner)	owner->stats->missed();
		health = 0;
	}
}

int Bullet::getSpeed() 	{ 
	// adding/subtracting range for a bit of acceleration
	if (flame) {
		return speed + range/40; 
	} else {
		return speed; 
	}	
}

void Bullet::startPos() {
	if (xposdir || yposdir) {
		// if we're not a mine, vanish if the player dies
		if (owner)	owner->stats->missed();
		health = 0;
	}
}

void Bullet::tookEnemy(Thing* x) {
	if (flame) {
		x->hurt(power*worldobj->delta/1000, xposdir, yposdir);
		range -= worldobj->delta; // range decreased in move() also
	} else {
		x->hurt(power, xposdir, yposdir);
		if (x->killsplayerbullet)
			this->health = 0;
	}
	if (owner) {
		owner->stats->hit();
		if (x->health <= 0)
			owner->stats->kill();
	}
}

void Bullet::tookObstacle(Thing* x) {
	if (x->killsplayerbullet)
		tookEnemy(x);		// I HATE CRATES
}


ThrownBomb::ThrownBomb(	Globals::sprindex _spr, int _xpos, int _ypos, 
		int _xdir, int _ydir)
		: Bullet(_spr, 0, _xpos, _ypos, _xdir, _ydir) 
{
	range = globals->loadInt("Constants/bombrange");	
	reflective = true;
}

void ThrownBomb::move(int delta) {
	Bullet::move(delta);
	Thing::move(delta);

	// bounce
	if (zpos == 0)
		jump(range/4);

	if (health <= 0) {
		Thing* se = new Bullet(Globals::SPARK, 10, xpos, ypos, 256, 256);
		Thing* e =  new Bullet(Globals::SPARK, 10, xpos, ypos, 256, 0);
		Thing* s =  new Bullet(Globals::SPARK, 10, xpos, ypos, 0, 256);
		Thing* n =  new Bullet(Globals::SPARK, 10, xpos, ypos, 0, -256);
		Thing* w =  new Bullet(Globals::SPARK, 10, xpos, ypos, -256, 0);
		Thing* nw = new Bullet(Globals::SPARK, 10, xpos, ypos, -256, -256);
		Thing* sw = new Bullet(Globals::SPARK, 10, xpos, ypos, -256, 256);
		Thing* ne = new Bullet(Globals::SPARK, 10, xpos, ypos, 256, -256);
		worldobj->addThing(se);
		worldobj->addThing(e);
		worldobj->addThing(s);
		worldobj->addThing(n);
		worldobj->addThing(w);
		worldobj->addThing(nw);
		worldobj->addThing(sw);
		worldobj->addThing(ne);
		health = 0;
	}
}

int ThrownBomb::getSpeed() {
	return globals->loadInt("Constants/bombspeed");
}


Spike::Spike(	Globals::sprindex _spr, int _xpos, int _ypos, 
		int _xdir, int _ydir)
		: Bullet(_spr, 0, _xpos, _ypos, _xdir, _ydir) 
{
	range = globals->loadInt("Constants/spikerange");	
	speed = globals->loadInt("Constants/spikespeed");
	health = globals->loadInt("Constants/spikehealth");
	orientated = true;
	frame = 3*(yposdir>>8) + (xposdir>>8) + 4;
}

int Spike::getSpeed() {
	return speed;
}


RoundBullet::RoundBullet( Globals::sprindex _spr, int type, int _xpos, int _ypos,
			int _xdir, int _ydir) : Thing(_spr) {

	optionalkill = true;
	killsplayerbullet = false;
	xpos = _xpos;	ypos = _ypos;
	xposdir = _xdir;	yposdir = _ydir;
	speed = globals->loadInt("Constants/roundbulletspeed");
	health = globals->loadInt("Constants/roundbullethealth");
}

void RoundBullet::move(int delta) {
	Thing::move(delta);

	xpos += (xposdir * getSpeed() * delta);
	ypos += (yposdir * getSpeed() * delta);

	if (XoutofArena() || YoutofArena()) {
		health = 0;
	}
}



