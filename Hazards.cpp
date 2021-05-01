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

#include <cmath>
#include <iostream>
#include "Thing.hh"
#include "Hazards.hh"
#include "Sprite.hh"
#include "Sound.hh"
#include "World.hh"
#include "Bullet.hh"
#include "Explosion.hh"

using namespace std;
extern World* worldobj;
extern Globals* globals;

/*
 * OBSTACLE
 ***********/

Obstacle::Obstacle(Globals::sprindex _spr) : Thing(_spr) {
	optionalkill = true;
	row = frame/4; // 4 frame animation for each fungi
	column = frame%4;
}

Obstacle::~Obstacle() {
	Sound::playSound("destroyobject");
}

void Obstacle::move(int delta) {
	frameremainder += delta;
	int timeperframe = getTimePerFrame();
	if (frameremainder > timeperframe) {
		frameremainder -= timeperframe;
		if (column < 3) column++;
		else column = 0;
		frame = 4*row + column;
	}
}

int Obstacle::getTimePerFrame() {
	return Thing::getTimePerFrame() * 4;
}

/* GRUNT
 ********/

int Grunt::age = 0;
int Grunt::numspeedup = 0;
int Grunt::agespeedup = 0;
bool Grunt::escapepod = false;
bool Grunt::randompod = false;
bool Grunt::bouncingpod = false;

Grunt::Grunt(Globals::sprindex _spr, int _speed) : Thing(_spr) {
	age = 0; agespeedup = 0; 
	speed = _speed;
	spr->hasorientation = true;
}

void Grunt::move(int delta) {
	Thing::move(delta);
	chase(delta);
}

void Grunt::tookPlayer(Thing* play) {
	play->hurt(100);
}

int Grunt::getSpeed() {
	return (int)((speed + agespeedup + numspeedup) * worldobj->monster_speed); 
}

void Grunt::calcSpeed(int delta, int numberkilled) {
	Grunt::age += delta;
	if (age > globals->loadInt("Constants/gruntaccelperiod")) {
		if (globals->verbosity > 0)
			cout << "Sped up grunts" << endl;
		agespeedup++;
		age = 0;
	}
	numspeedup = numberkilled / globals->loadInt("Constants/gruntaccelnumber");
}

void Grunt::hurt(int injury, int xdir, int ydir) {
	if ((health - injury <= 0) && escapepod) {
		if (randompod) {
			int randdir = (int)(9.0*rand() / RAND_MAX);
			if (randdir == 4) randdir = 0;
			xdir = (randdir/3) - 1;	xdir *= 256;
			ydir = (randdir%3) - 1;	ydir *= 256;
		} else {
			// need to quantise the direction so the pod doesn't 
			// end up looking for a non-existant frame number
			xdir = quantiseDirection(xdir);
			ydir = quantiseDirection(ydir);
		}

		Thing* pod;
		if (bouncingpod) {
			pod = new Ball(Globals::POD);
			pod->xposdir = xdir;
			pod->yposdir = ydir;
			pod->xpos = xpos;
			pod->ypos = ypos;
		} else {
			pod = new Spike(Globals::POD, xpos, ypos, -xdir, -ydir);	
		}
		worldobj->addThing(pod);
	}
	Thing::hurt(injury, xdir, ydir);
}

/* BALL
 *******/

class Sprite;

Ball::Ball(Globals::sprindex _spr) : Thing(_spr) {
	speed = globals->loadInt("MonsterSpecs/Ball/speed");
	if ((int)2*rand() / RAND_MAX)
		xposdir = 256;
	else
		xposdir = -256;
	if ((int)2*rand() / RAND_MAX)
		yposdir = 256;
	else
		yposdir = -256;

	animpingpong = false;
}

void Ball::move(int delta) {
	Thing::move(delta);
	int displacement = (int)(getSpeed() * delta);
	
	xpos += displacement * xposdir >> 8;
	ypos += displacement * yposdir >> 8;

	bounce();
}

void Ball::tookEnemy(Thing* x) {
	xposdir = (x->xpos < xpos)*512 - 256;	// 256 or -256
	yposdir = (x->ypos < ypos)*512 - 256;
	Sound::playSound("ballbounce");
}

void Ball::tookPlayer(Thing* x) {
	x->hurt(100);
}

void Ball::recoil(int injury, int xdir, int ydir) {
	if (xdir < 0)		xposdir = -256;
	else if (xdir == 0)	;
	else			xposdir = 256;
	if (ydir < 0)		yposdir = -256;
	else if (ydir == 0)	;
	else			yposdir = 256;
}

int Ball::getTimePerFrame() {
	return Thing::getTimePerFrame() / 4;
}

/*
 * SPIKER
 *********/

Spiker::Spiker(Globals::sprindex _spr) : Thing(_spr) {
	health = globals->loadInt("MonsterSpecs/Spiker/health");
	speed = globals->loadInt("MonsterSpecs/Spiker/speed");
	spr->hasorientation = true;
}

void Spiker::move(int delta) {
	Thing::move(delta);
	chase(delta);

	if (health <= 0) {
		Thing* ne = new Spike(Globals::SPIKE, xpos, ypos, 256, -256);	
		Thing* nw = new Spike(Globals::SPIKE, xpos, ypos, -256, -256);	
		Thing* se = new Spike(Globals::SPIKE, xpos, ypos, 256, 256);	
		Thing* sw = new Spike(Globals::SPIKE, xpos, ypos, -256, 256);	
		worldobj->addThing(ne);
		worldobj->addThing(nw);
		worldobj->addThing(se);
		worldobj->addThing(sw);
		Sound::playSound("destroyspiker");
	}
}

int Spiker::getTimePerFrame() {
	return Thing::getTimePerFrame() / 4;
}

/*
 * ASTEROID
 ************/

Asteroid::Asteroid() : Thing(Globals::ASTEROID) {
	speed = globals->loadInt("MonsterSpecs/Asteroid/speed");
	size = 2; recoiltimer = 0;
	health = globals->loadInt("MonsterSpecs/Asteroid/bigglobhealth");
	wobbleamplitude = globals->loadInt("MonsterSpecs/Asteroid/wobbleamplitude");
	wobblespeed = globals->loadInt("MonsterSpecs/Asteroid/wobblespeed");
	animframe = 0;
}

Asteroid::Asteroid(int _xpos, int _ypos, int _size, int _xdir, int _wobblestart) 
		: Thing(Globals::ASTEROID) 
{
	speed = globals->loadInt("MonsterSpecs/Asteroid/speed");
	xwobblecent = _xpos; xposdir = _xdir;
	ywobblecent = _ypos;
	size = _size;
	circletime = _wobblestart;
	health = globals->loadInt("MonsterSpecs/Asteroid/health");
	wobbleamplitude = globals->loadInt("MonsterSpecs/Asteroid/wobbleamplitude");
	wobblespeed = globals->loadInt("MonsterSpecs/Asteroid/wobblespeed");
	animframe = 0;
	recoiltimer = 1000;
}

void Asteroid::move(int delta) {
	wobbleChase(delta);
	if (recoiltimer > 0) {
		// xpos is going to be set to it's wobble center anyway
		xwobblecent += (int)(xposdir * getSpeed() * delta);
		recoiltimer -= delta;
	 }
	
	if (circletime > 2*M_PI) circletime = 0;
	
	if (animframe < (int)(spr->getFrames()/3)-1)
		animframe++;
	else
		animframe = 0;
	
	frame = animframe + (spr->getFrames()/3) * size;

	if (health <= 0) {
		switch (size) {
			case 2:
				firstSplit();
				break;

			case 1:
				secondSplit();
				break;
				
			case 0:
				explode(0, Globals::GOREEXPLODE);
				break;
		}
	}
}

void Asteroid::firstSplit() {
	Thing* ne = new Asteroid(xpos, ypos, 1, 1, 0);	
	Thing* nw = new Asteroid(xpos, ypos, 1, -1, 0);	
	Thing* se = new Asteroid(xpos, ypos, 1, 1, M_PI);	
	Thing* sw = new Asteroid(xpos, ypos, 1, -1, M_PI);
	worldobj->addThing(ne);
	worldobj->addThing(nw);
	worldobj->addThing(se);
	worldobj->addThing(sw);
	Sound::playSound("asteroidsplit");
}

void Asteroid::secondSplit() {
	Thing* e = new Asteroid(xpos, ypos, 0, 1, 0);	
	Thing* w = new Asteroid(xpos, ypos, 0, -1, M_PI);	
	worldobj->addThing(e);
	worldobj->addThing(w);
	Sound::playSound("asteroidsplit");
}

/*
 * SNIPER
 *********/

Sniper::Sniper(Globals::sprindex _spr) : Thing(_spr) {
	speed = globals->loadInt("MonsterSpecs/Sniper/speed");
	remainder = -5000;
	remaindermax = globals->loadInt("MonsterSpecs/Sniper/reloadtime");
	animpingpong = false;
}

void Sniper::move(int delta) {
	Thing::move(delta);
	seekWalls(delta);
	randomMove(2*delta, 2000);
	keepInArena();

	remainder += delta;
	if (remainder > remaindermax) {
		remainder -= remaindermax;
		
		Drone* drone = new Drone(Globals::SNIPER);
		drone->xpos = xpos; drone->ypos = ypos;
		worldobj->addThing(drone);
		Sound::playSound("launchdrone");
	}
}

/*
 * RIGHTANGLER
 ***************/

RightAngler::RightAngler(Globals::sprindex _spr, bool _verthunting) : Thing(_spr) {
	verthunting = _verthunting;
	goingup = (rand() < RAND_MAX/2);

	huntspeed = globals->loadInt("MonsterSpecs/RightAngler/huntspeed");
	attackspeed = globals->loadInt("MonsterSpecs/RightAngler/attackspeed");
	startPos();
	orientation = 0;
}

void RightAngler::startPos() {
	Thing::randomPos();
	
	attacking = false;
	speed = huntspeed;

	if (!goingup) {
		if (verthunting) {
			xpos = 0;
		} else {
			ypos = 0;
		}
	} else {
		if (verthunting) {
			xpos = ARENAWIDTH - spr->width;
		} else {
			ypos = ARENAHEIGHT - spr->height;
		}
	}
}

void RightAngler::move(int delta) {
	Thing::move(delta);
	int rowsize = spr->getFrames()/4;
	frame = frame % rowsize;
	frame += orientation * rowsize;

	Thing* p = worldobj->getNearestPlayer(xpos, ypos);

	// I've parameterised the dimensions so some will rest on horiz walls, others on vert walls
	int huntdim, hunttarget, attackdim, maxattackdim;
	
	if (verthunting) {
		huntdim = ypos; hunttarget = p->ypos;
		attackdim = xpos; maxattackdim = ARENAWIDTH - spr->width;
	} else {
		huntdim = xpos; hunttarget = p->xpos;
		attackdim = ypos; maxattackdim = ARENAHEIGHT - spr->height;
	}

	int displacement;
	displacement = getSpeed() * delta;

	if (attacking) {
		if (goingup)
			attackdim -= displacement;
		else
			attackdim += displacement;

		// lunge until you hit the oppisite wall
		if (attackdim < 0) {
			attacking = false;
			speed = huntspeed;
			goingup = false;
			ypos = 0;
		} else if (attackdim > maxattackdim) {
			attacking = false;
			speed = huntspeed;
			goingup = true;
			attackdim = maxattackdim;
		}
		
	} else {
		if (abs(hunttarget - huntdim) < 2000) {
			attacking = true;
			speed = attackspeed;
			Sound::playSound("rightanglerlunge");
		} else {
			// chase in the hunting dimension
			if (hunttarget < huntdim) {
				huntdim -= displacement;
			} else {
				huntdim += displacement;
			}
		}
	}
	
	// yuk. This is a job for references
	if (verthunting) {
		if (huntdim < ypos)	orientation = 0; // north
		if (huntdim > ypos)	orientation = 2; // south
		if (attackdim < xpos)	orientation = 3; // west
		if (attackdim > xpos)	orientation = 1; // east
		ypos = huntdim; xpos = attackdim;
	} else {
		if (attackdim < ypos)	orientation = 0; // north
		if (attackdim > ypos)	orientation = 2; // south
		if (huntdim < xpos)	orientation = 3; // west
		if (huntdim > xpos)	orientation = 1; // east
		xpos = huntdim; ypos = attackdim;
	}
}

void RightAngler::tookObstacle(Thing* obst) {
	Thing::tookObstacle(obst);
	attacking = false;
	verthunting = !verthunting;
	speed = huntspeed;
}

/*
 * BOSS I
 *********/

SimpleBoss::SimpleBoss(Globals::sprindex _spr, int _number) : Thing(_spr) {
	speed = globals->loadInt("MonsterSpecs/SimpleBoss/speed");
	remainder = 0; 
	health = globals->loadInt("MonsterSpecs/SimpleBoss/health");
	wobbleamplitude = (ARENAHEIGHT - spr->height)/2;
	wobblespeed = globals->loadInt("MonsterSpecs/SimpleBoss/wobblespeed");
	reloadtime = globals->loadInt("MonsterSpecs/SimpleBoss/reloadtime");
	number = _number;
	startPos();
}

void SimpleBoss::startPos() {
	if (number%2 == 0) xwobblecent = 4000; 
	else xwobblecent = ARENAWIDTH - 4000 - spr->width;
	circletime = M_PI * number / 2;
	move(0);	// update xpos and ypos
}

void SimpleBoss::hurt(int injury, int xdir, int ydir) {
	if (injury >= 100) {
		Thing* e = new Explosion(Globals::METALLICEXPLODE,
					xpos, ypos, xdir, ydir);
		worldobj->addThing(e);
		Sound::playSound("hurt");
	}
	Thing::hurt(injury, xdir, ydir);
}

void SimpleBoss::move(int delta) {
	Thing::move(delta);

	wobbleChase(delta);
	ywobblecent = (ARENAHEIGHT - spr->height)/2;
	//circletime += (worldobj->monster_speed * delta * wobblespeed)/5000;
	
	remainder += delta;
	if (remainder > reloadtime) {
		remainder -= reloadtime;
		if ((worldobj->getNearestPlayer(xpos, ypos))->xpos > xpos) {
			xposdir = 256;
		} else {
			xposdir = -256;
		}
		
		Spike* enemybullet = new Spike(Globals::SPIKE, 
						xpos, ypos+spr->height/2, xposdir, 0);
		worldobj->addThing(enemybullet);
		Sound::playSound("enemyfire");
	}
}

void SimpleBoss::tookObstacle(Thing* obst) {
	// it can push crates
	obst->tookObstacle(this);
}

/*
 * DRONE
 ********/

Drone::Drone(Globals::sprindex _spr, Thing* _target, bool soaksbullets) : Thing(_spr) {
	speed = globals->loadInt("MonsterSpecs/Drone/speed");
	target = _target;
	timebetweenlook = 1500;
	killsplayerbullet = soaksbullets;
	animpingpong = false;
}

Drone::~Drone() {
	if (killsplayerbullet)	// don't award a bonus if we get killed by natural causes
		bonusGen(2, xpos, ypos);
	//explode(0, Globals::METALLICEXPLODE);
}

void Drone::move(int delta) {
	Thing::move(delta);
	chase(delta, timebetweenlook, target);

	if (target) {
		// invhypot is used for normalising the chase angle, so it's
		// the inverse of the distance.
		if (invhypot > 0.0005) {
			// reached target, suicide
			health = 0;
		}
		if (target->health == 0)
			health = 0;
	}
}

void Drone::startPos() {
	health = 0;
}


/*
 * ROCKMEN
 **********/

RockMan::RockMan(Globals::sprindex _spr) : Thing(_spr) {
	speed = globals->loadInt("MonsterSpecs/RockMan/speed");
	optionalkill = true;
}

void RockMan::hurt(int injury, int xdir, int ydir) {
	recoil(injury, xdir, ydir);
	Sound::playSound("hurtinvincible");
}

void RockMan::move(int delta) {
	chase(delta);
	keepInArena();
}

/*
 * TURRET
 *********/

Turret::Turret(Globals::sprindex _spr, int _fansize) : Thing(_spr) {
	health = globals->loadInt("MonsterSpecs/Turret/health");
	fansize = _fansize;
	lookremainder = -globals->loadInt("MonsterSpecs/Turret/firstfiredelay");
	reloadtime = globals->loadInt("MonsterSpecs/Turret/reloadtime");
	bulletspeed = globals->loadInt("MonsterSpecs/Turret/bulletspeed");
}

void Turret::move(int delta) {
	Thing::move(delta);
	lookremainder += delta;
	if (lookremainder >= reloadtime) {
		lookremainder -= reloadtime;

		// want bullet's speed proportional to player distance
		Thing* p = worldobj->getNearestPlayer(xpos, ypos);
		xposdir = p->xpos - xpos;
		yposdir = p->ypos - ypos;
		xposdir >>= 14;
		yposdir >>= 14; // 14 is arbitrary. Wanted something not too rounded down, yet not large enough to be really granular

		Thing* bullet = new RoundBullet(Globals::ROUNDBULLET, 0, xpos, ypos, xposdir, yposdir);
		bullet->speed = bulletspeed;
		worldobj->addThing(bullet);

		if (fansize) {
			for(int i=1; i<fansize; i++) {
				Thing* bullet = new RoundBullet(Globals::ROUNDBULLET, 0, xpos, ypos, xposdir+i, yposdir+i);
				bullet->speed = bulletspeed;
				worldobj->addThing(bullet);
			}
		}
		Sound::playSound("enemyfire");
	}
}

/*
 * SOLDIER
 **********/

bool Soldier::playedfire = false;

Soldier::Soldier(Globals::sprindex _spr) : Thing(_spr) {
	health = globals->loadInt("MonsterSpecs/Soldier/health");
	speed = globals->loadInt("MonsterSpecs/Soldier/speed");
	timebetweenlook = globals->loadInt("MonsterSpecs/Soldier/timebetweenlook");
	bulletspeed = globals->loadInt("MonsterSpecs/Soldier/bulletspeed");
}

void Soldier::move(int delta) {
	Thing::move(delta);

	// at every direction change, fire
	if (lookremainder+delta >= timebetweenlook) {
		Thing* p = worldobj->getNearestPlayer(xpos, ypos);
		int xaim = p->xpos - xpos;
		int yaim = p->ypos - ypos;
		xaim >>= 14;
		yaim >>= 14;

		Thing* bullet = new RoundBullet(Globals::ROUNDBULLET, 0, xpos, ypos, xaim, yaim);
		bullet->speed = bulletspeed;
		worldobj->addThing(bullet);

		if (!playedfire) {
			Sound::playSound("enemyfire");
			playedfire = true;
		}
	} else {
		playedfire = false;
	}

	// no, I don't have a very high opinion of the military
	randomMove(delta, timebetweenlook);
	keepInArena();
}

/*
 * CIRCLING BOSS
 ****************/

CircleBoss::CircleBoss(Globals::sprindex _spr, int x, int y, int _speed, float _start) : Thing(_spr) {
	health = globals->loadInt("MonsterSpecs/CircleBoss/health");
	wobblespeed = globals->loadInt("MonsterSpecs/CircleBoss/wobblespeed");
	wobbleamplitude = globals->loadInt("MonsterSpecs/CircleBoss/wobbleamplitude");
	animpingpong = globals->manager->get_integer_resource("MonsterSpecs/CircleBoss/animpingpong", 1);
	circletime = start = _start;
	spawnspeed = _speed;
	origxwobblecent = x - spr->width/2;
	origywobblecent = y - spr->height/2;
	startPos();
}

void CircleBoss::startPos() {
	xwobblecent = origxwobblecent; 
	ywobblecent = origywobblecent; 
	move(0);
}

void CircleBoss::move(int delta) {
	Thing::move(delta);

	circletime += (worldobj->monster_speed * delta * wobblespeed)/10000;
	if (circletime > (2*M_PI + start)) {
		circletime = start;
		Thing* bullet = new Grunt(Globals::DRONE, spawnspeed);
		bullet->xpos = xpos; bullet->ypos = ypos;
		worldobj->addThing(bullet);
		Sound::playSound("launchdrone");
	}

	XCosMove();	YSinMove();
}

void CircleBoss::hurt(int injury, int xdir, int ydir) {
	if (injury >= 100) {
		Thing* e = new Explosion(Globals::METALLICEXPLODE,
					xpos, ypos, xdir, ydir);
		worldobj->addThing(e);
		Sound::playSound("hurt");
	}
	Thing::hurt(injury, xdir, ydir);
}

void CircleBoss::tookObstacle(Thing* obst) {
	// it can push crates
	obst->tookObstacle(this);
}

/*
 * CENTIPEDE
 ************/

Centipede::Centipede(Globals::sprindex _spr, Thing* attachedto) : Thing(_spr) {
	health = globals->loadInt("MonsterSpecs/Centipede/health");
	speed = globals->loadInt("MonsterSpecs/Centipede/speed");
	turningcirc = globals->loadInt("MonsterSpecs/Centipede/turningcirc");
	seekaccuracy = globals->loadInt("MonsterSpecs/Centipede/seekaccuracy");
	maxfeartimer = globals->loadInt("MonsterSpecs/Centipede/fearinterval");
	gap = globals->loadInt("MonsterSpecs/Centipede/segmentgap");
	feartimer = maxfeartimer;
	boss = next = 0;
	prev = attachedto;
	if (prev)	prev->next = this;
	// startPos();	centipede maker has to call this.
	// (I can't call it here because getInBehind would only flow through
	// an incomplete list structure.
}

Centipede::~Centipede() {
	// should recode this in terms of detach()
	if (next) {
		next->health = 0; // tail from here dies
	}
	if (boss) {
		boss->detach(this);
	}
	bonusGen(3, xpos, ypos);
	explode(0, Globals::GOREEXPLODE);
}

void Centipede::hurt(int injury, int xdir, int ydir) {
	feartimer = 0;
	if (injury > 50) Sound::playSound("hurt");
	Thing::hurt(injury, xdir, ydir);
}

void Centipede::move(int delta) {
	Thing::move(delta);
	if (prev) {
		// follow the leader
		xposdir = prev->xpos - xpos;
		yposdir = prev->ypos - ypos;

		/* speed proportional to distance
		   (i'm using speed not getSpeed, because I don't want
		   repel powerups to affect tension)
		*/
		int displacement = speed * delta;
		xpos += displacement * xposdir / gap;
		ypos += displacement * yposdir / gap;
	} else {
		feartimer += delta;
		if (feartimer > maxfeartimer) {
			feartimer = maxfeartimer;
			chase(delta, seekaccuracy);
		} else {	// I'm hit!
			seekWalls(delta);
		}
	}
	bounce();
}

void Centipede::getInBehind() {
	int gap = globals->loadInt("MonsterSpecs/Centipede/segmentgap");
	chase(0, 0, prev); // point in the right direction
	xpos = (int)(xposdir * gap * invhypot) + prev->xpos;
	ypos = (int)(yposdir * gap * invhypot) + prev->ypos;

	if (next)	next->getInBehind();
}

void Centipede::startPos() {
	if (!prev) {		// if we're the leader
		randomPos();
		chase(0, 0); // pick a direction, but don't go anywhere yet
		if (next)	next->getInBehind();
	}
}

/*
 * CELLPHONES
 *************/

Cellphone::Cellphone(Globals::sprindex _spr, Thing* attachedto) : Thing(_spr) {
	health = globals->loadInt("MonsterSpecs/Cellphone/health");
	speed = globals->loadInt("MonsterSpecs/Cellphone/speed");
	timebetweenlook = globals->loadInt("MonsterSpecs/Cellphone/timebetweenlook");
	dronespeed = globals->loadInt("MonsterSpecs/Cellphone/signalspeed");
	next = 0;
	prev = attachedto;
	if (prev)	prev->next = this;
}

Cellphone::~Cellphone() {
	bonusGen(3, xpos, ypos);
	explode(0, Globals::METALLICEXPLODE);
}

void Cellphone::move(int delta) {
	Thing::move(delta);
	seekWalls(delta/2);
	keepInArena();
	if ((lookremainder+delta) > timebetweenlook) {
		Drone* drone;
		drone = 0;
		if (next) {
			// false means they don't soak up player bullets
			drone = new Drone(Globals::SIGNAL, next, false);
		} else if (prev)
			drone = new Drone(Globals::SIGNAL, prev, false);
			
		if (drone) {
			drone->xpos = xpos; drone->ypos = ypos;
			drone->timebetweenlook = 0;
			drone->speed = dronespeed;
			worldobj->addThing(drone);
		}
	}
	randomMove(2*delta, timebetweenlook);
}

/*
 * SPIRAL
 *********/

Spiral::Spiral(Globals::sprindex _spr) : Thing(_spr) {
	health = globals->loadInt("MonsterSpecs/Spiral/health");
	speed = globals->loadInt("MonsterSpecs/Spiral/speed");
	starttime = globals->loadInt("MonsterSpecs/Spiral/minstarttime");
	expandstep = globals->loadInt("MonsterSpecs/Spiral/expandstep");
	starttime += (int)((float)globals->loadInt("MonsterSpecs/Spiral/startvariation") * rand() / RAND_MAX);
	spr->hasorientation = true;
	startPos();
}

void Spiral::move(int delta) {
	Thing::move(delta);

	circletime += (double)(getSpeed() * delta)/1000;
	int expansion = (int)(2 * circletime) - starttime;
	if (expansion < 1)	expansion = 1;

	XCosMove(expansion*expandstep);
	YSinMove(expansion*expandstep);
	keepInArena();
}

/*
 * LASERS
 *********/

Laser::Laser(Globals::sprindex _spr, bool _vertical) : Thing(_spr) {
	health = 1000;
	firetime = 0;
	brightness = 3;
	speed = globals->loadInt("MonsterSpecs/Laser/speed");
	wobblespeed = globals->loadInt("MonsterSpecs/Laser/wobblespeed");
	wobbleamplitude = globals->loadInt("MonsterSpecs/Laser/wobbleamplitude");
	firingperiod = globals->loadInt("MonsterSpecs/Laser/firingperiod");
	reloadperiod = globals->loadInt("MonsterSpecs/Laser/reloadperiod");
	vertical = _vertical;
	startPos();
}

void Laser::move(int delta) {
	wobbleChase(delta, !vertical);
	firetime += delta;
	if (firetime > firingperiod)
		firetime = -reloadperiod;
	if (	(firetime - delta) < 0 &&
		firetime > 0)
			Sound::playSound("firelaser");
}

void Laser::startPos() {
	firetime = -firingperiod; 
	int min, max, firingdim;
	if (vertical) {
		min = ARENAHEIGHT/2 - globals->loadInt("Constants/barepatchheight");
		max = ARENAHEIGHT/2 + globals->loadInt("Constants/barepatchheight");
		firingdim = ypos;
	} else {
		min = ARENAWIDTH/2 - globals->loadInt("Constants/barepatchwidth");
		max = ARENAWIDTH/2 + globals->loadInt("Constants/barepatchwidth");
		firingdim = xpos;
	}
	
	do {
		randomPos();
	} while( firingdim < min && firingdim > max);

	xwobblecent = xpos;	ywobblecent = ypos;
}

bool Laser::colcheck(Thing* thingb) {
	// is it touching the barrel of the laser?
	if (Thing::colcheck(thingb))	return true;
	
	if ((thingb->getType() == Thing::PLAYER) && (firetime > 0)) {
		int position;
		if (vertical) {
			position = spr->width/2 + xpos;
			if 	(thingb->xpos < position && 
				(thingb->xpos + thingb->spr->width > position))
				return true;
		} else {
			position = spr->height/2 + ypos;
			if 	(thingb->ypos < position && 
				(thingb->ypos + thingb->spr->height > position))
				return true;
		}
	}
	return false;
}

void Laser::draw() {
	if (firetime > 0) {
		if (brightness == 3) brightness = 6;
		else 		brightness = 3;
		
		int position;
		if (vertical) {
			position = (spr->width/2 + xpos)>>8;
			SDL_SetRenderDrawColor(game_renderer, 255, ((float)brightness/10) * 255, ((float)brightness/10) * 255, 0.8f * 255);
			SDL_RenderDrawLine(game_renderer,position, 0, position, ARENAHEIGHT>>8);
/*			CL_Display::draw_line(	position, 0, position, ARENAHEIGHT>>8,
						1.0f, (float)brightness/10, (float)brightness/10, 0.8f );*/
		} else {
			position = (spr->height/2 + ypos)>>8;
			SDL_SetRenderDrawColor(game_renderer, 255, ((float)brightness/10) * 255, ((float)brightness/10) * 255, 0.8f * 255);
			SDL_RenderDrawLine(game_renderer, 0, position, ARENAWIDTH>>8, position);
/*			CL_Display::draw_line(	0, position, ARENAWIDTH>>8, position,
						1.0f, (float)brightness/10, (float)brightness/10, 0.8f );*/
		}
	}
	Thing::draw();
}

void Laser::hurt(int injury, int _xdir, int _ydir) {
	recoil(injury, _xdir, _ydir);
	if (injury > 50) Sound::playSound("hurtinvincible");
	if (vertical) {
		if (_xdir != 0) 	xwobblecent = xpos;
		if (_xdir > 0)		circletime = 3*M_PI/2; 
		else if (_xdir < 0)	circletime = M_PI/2;
	} else {
		if (_ydir != 0)		ywobblecent = ypos;
		if (_ydir > 0)		circletime = 0; 
		else if (_ydir < 0)	circletime = M_PI;
	}
}
