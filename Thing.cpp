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

#include "Thing.hh"
#include "Sprite.hh"
#include "Sound.hh"
#include "World.hh"
#include "Bonus.hh"
#include "Barricade.hh"
#include "Explosion.hh"
#include "Display.hh"
#include "IRandom.hh"
#include <cmath>
#include <iostream>

using namespace std;
extern World* worldobj;
extern Globals* globals;

int Thing::recoildistance = 0;

Thing::Thing() {
}

Thing::Thing(Globals::sprindex _spr) {
	setSprite(_spr);
	health = 100; 
	zpos = zvelocity = speed = lookremainder = 0;
	xposdir = yposdir = 0;
	circletime = invhypot = 0;
	next = prev = 0;
	levchangepersistant = dropsbarricade = optionalkill = false;
	killsplayerbullet = true;
	gap = 4000;
	if (!recoildistance)	recoildistance = globals->loadInt("Constants/recoildistance");

	startPos();
}

void Thing::setSprite(Globals::sprindex newspr) {
	sprnumber = newspr;
	spr = globals->spr[newspr];

	setupAnim();
}

void Thing::startPos() {
	randomPos();
	circletime = 0;
}

void Thing::setupAnim() {
	frame = (unsigned int)(IRandom::roll(spr->getFrames()));
	
	frameremainder = 0;

	animdirection = 1;
	animpingpong = true;
}

Thing::~Thing() { 
	if (globals->verbosity > 1)
		cout << "Thing " << this << " destroyed" << endl; 
	if (dropsbarricade) {
		Thing* corpse = new Barricade(Globals::TRASH, xpos, ypos);
		Thing* corpseimp = new BarricadeImpact(xpos, ypos);
		worldobj->addThing(corpse);
		worldobj->addThing(corpseimp);
	}

	// if we're a linked list monster...
	if (next)	next->detach(this);
	if (prev)	prev->detach(this);
}

void Thing::bonusGen(int x, int y, int luck) {
	Bonus::randGen(x, y, luck);
}

int Thing::getTimePerFrame() {
	return spr->timeperframe;
}

void Thing::explode(int size, Globals::sprindex type) {
	Sound::playSound("explode", ((float)xpos / ARENAWIDTH) * 2 - 1);
	worldobj->flashing = 40;
	
	Explosion::largeExplosion(type, size, xpos, ypos);
	//Thing* e = new Explosion(type, xpos, ypos, 0, 0);
	//worldobj->addThing(e);
}

void Thing::draw() {
	facingleft = false;
	if (xposdir < 0)	facingleft = true;

	if (worldobj->monster_size) {
		spr->draw(xpos, ypos-zpos, frame, facingleft, worldobj->monster_size, worldobj->monster_size);
	} else {
		// ypos-zpos because low numbers are high on the screen.
		spr->draw(xpos, ypos-zpos, frame, facingleft);
	}
}

/* When you're overriding this for more interesting enemy movement
	remember to chain through the parents of the method */
	
void Thing::move(int delta) {
	movez(delta);
	
	frameremainder += delta;
	int timeperframe = getTimePerFrame();
	if (frameremainder > timeperframe) {
		frameremainder -= timeperframe;

		// advance frame
		frame += animdirection;
		if (frame >= spr->getFrames() - 1) {
			if (animpingpong) {
				animdirection = -1;
				frame = spr->getFrames() - 1;
			} else if (frame >= spr->getFrames()) {
				frame = 0;
			}
		} else if (frame <= 0) {
			animdirection = 1;
			frame = 0;	
		}
	}
}

void Thing::movez(int delta) {
	if (zpos > 0) {
		zvelocity -= delta * globals->loadInt("Constants/gravity");
		zpos += zvelocity * delta;
		if (zpos < 0) { zpos = 0; zvelocity = 0; }
	}
}

void Thing::jump(int maxvelocity) {
	//zvelocity = (int)(IRandom::roll(maxvelocity));
	zvelocity = maxvelocity;
	zpos = 2000;
}

int Thing::getXCenter() {
	return xpos + spr->width/2;
}

int Thing::getYCenter() {
	return ypos + spr->height/2;
}

void Thing::randomPos() {
	int maxx = ARENAWIDTH - spr->width;
	int maxy = ARENAHEIGHT - spr->height;
	int playerposx = ARENAWIDTH/2 + (globals->spr[Globals::PLAYERONE]->width)/2;
	int playerposy = ARENAHEIGHT/2 + (globals->spr[Globals::PLAYERONE]->height)/2;
	do {
		xpos=(int)(IRandom::roll(maxx));
		ypos=(int)(IRandom::roll(maxy));
	} while( !(abs(playerposx - xpos) > globals->loadInt("Constants/barepatchwidth") ) &&
		!(abs(playerposy - ypos) > globals->loadInt("Constants/barepatchheight") ) );
	
	xwobblecent = xpos;	ywobblecent = ypos;
}

void Thing::tookObstacle(Thing* obst) {
	/* This is a really weird algorithm, it calculates the bounding box
	 * overlap, and makes a force proportional to how close to the center 
	 * of the obstacle.
	 */
	int xd, yd = 0;
	int xforce, yforce = 0;

	if (getXCenter() > obst->getXCenter()) {
		// case where obstacle is on left (xd should be positive)
		//   ( Obstacle's right side       ) - our left side
		xd = (obst->xpos + obst->spr->width) - xpos;
	} else {
		// case where obstacle is on right (xd negative)
		//   obst's left - our right
		xd = obst->xpos - (xpos + spr->width);
	}

	// same as above, except in the Y dimension
	if (getYCenter() > obst->getYCenter()) {
		yd = (obst->ypos + obst->spr->height) - ypos;
	} else {
		yd = obst->ypos - (ypos + spr->height);
	}

	// swap the two overlaps, but keep their original sign.
	// This is supposed to approximate a force vector from the 
	// center of the obstacle
	if (xd > 0)	xforce = abs(yd);
	else		xforce = -abs(yd);
	if (yd > 0)	yforce = abs(xd);
	else		yforce = -abs(xd);

	recoil(8, xforce, yforce);
}

void Thing::hurt(int injury, int xdir, int ydir) {
	health -= injury;
	recoil(injury, xdir, ydir);
	if (health > 0 && injury > 50) {
		Sound::playSound("thump");
	}
}

void Thing::recoil(int injury, int xdir, int ydir) {
	xpos += (xdir * injury * recoildistance) >> 8;
	ypos += (ydir * injury * recoildistance) >> 8;
	// something for the circling types
	//circletime -= (worldobj->monster_speed * injury)/1000;
	xwobblecent += (xdir * injury * recoildistance) >> 8;
	ywobblecent += (ydir * injury * recoildistance) >> 8;
}

void Thing::pull(int delta, int xd, int yd) {
	// generalise this to replace centipede AI someday
	xpos += xd;
	ypos += yd;

	if (prev) {
		xd = xpos - prev->xpos;
		yd = ypos - prev->ypos;
		prev->pull(	delta, delta * xd * getSpeed() / gap, 
				delta * yd * getSpeed() / gap);
	}
}

void Thing::detach(Thing* t) {
	if (prev == t)	prev = NULL;
	else if (next == t)	next = NULL;
	else cout << "Was asked to detach something not connected" << endl;
}

double hypotenuse(int ai, int bi) {
	double a = (double)ai;	// overkill
	double b = (double)bi;
	return sqrt(a*a + b*b);
}

void Thing::normaliseDirection() {
	double d = hypotenuse(xposdir, yposdir);
	if (!d)	d = 1;

	invhypot = 1 / d;
}

void Thing::chase(int delta, int timebetweenlook, Thing* target) {
	if (timebetweenlook) lookremainder += delta;
	if (lookremainder >= timebetweenlook) {
		lookremainder -= timebetweenlook;
		
		if (!target) // default target
			target = worldobj->getNearestPlayer(xpos, ypos);
			
		// change direction
		xposdir = target->xpos - xpos;
		yposdir = target->ypos - ypos;
		normaliseDirection();
	}
	int displacement = getSpeed() * delta;
	xpos += (int)(displacement * xposdir * invhypot);
	ypos += (int)(displacement * yposdir * invhypot);
}

void Thing::randomMove(int delta, int timebetweenlook) {
	if (timebetweenlook) lookremainder += delta;
	if (lookremainder >= timebetweenlook) {
		lookremainder -= timebetweenlook;
		
		xposdir = IRandom::roll(RAND_MAX) - (RAND_MAX/2);
		yposdir = IRandom::roll(RAND_MAX) - (RAND_MAX/2);
		xposdir >>= 16;
		yposdir >>= 16;
		normaliseDirection();
	}

	int displacement = getSpeed() * delta;
	xpos += (int)(displacement * xposdir * invhypot);
	ypos += (int)(displacement * yposdir * invhypot);
}

void Thing::seekWalls(int delta) {
	int displacement = getSpeed() * delta;

	if (xpos < ARENAWIDTH/2) {
		xpos -= displacement;
	} else {
		xpos += displacement;
	}

	if (ypos < ARENAHEIGHT/2) {
		ypos -= displacement;
	} else {
		ypos += displacement;
	}
}

// Sends the center of the wobbling in pursuit of the player
// (doesn't do any actual movement)
void Thing::centerChase(int delta) {
	int displacement = getSpeed() * delta;
	Thing* target = worldobj->getNearestPlayer(xpos, ypos);

	if ((target->xpos) < (this->xwobblecent))
		xwobblecent -= displacement;
	else
		xwobblecent += displacement;

	if ((target->ypos) < (this->ywobblecent))
		ywobblecent -= displacement;
	else
		ywobblecent += displacement;
}

// Chase the player and wobble
void Thing::wobbleChase(int delta, bool vertwobble) {
	centerChase(delta);
	if (vertwobble)	{
		YSinMove();
		xpos = xwobblecent;
	} else { 
		XCosMove();
		ypos = ywobblecent;
	}
	circletime += (worldobj->monster_speed * delta * wobblespeed)/10000;
}

void Thing::XCosMove(int amplitude) {
	int oldxpos = xpos;
	if (amplitude)
		xpos = (int)(cos(circletime) * amplitude) + xwobblecent;
	else
		xpos = (int)(cos(circletime) * wobbleamplitude) + xwobblecent;
	if (xpos - oldxpos != 0)
		xposdir = xpos - oldxpos;
}

void Thing::YSinMove(int amplitude) {
	if (amplitude)
		ypos = (int)(sin(circletime) * amplitude) + ywobblecent;
	else
		ypos = (int)(sin(circletime) * wobbleamplitude) + ywobblecent;
}

int Thing::getSpeed() {
	return (int)(worldobj->monster_speed * speed);
}

int Thing::XoutofArena(bool includespr) {
	if (includespr) {
		if ((xpos < 0))
			return -1;
		if ((xpos + spr->width) > ARENAWIDTH)
			return 1;
		return 0;
	} else {
		if ((xpos < -(spr->width)))
			return -1;
		if (xpos > ARENAWIDTH)
			return 1;
		return 0;
	}
}

int Thing::YoutofArena(bool includespr) {
	if (includespr) {
		if ((ypos < 0))
			return -1;
		if ((ypos + spr->height) > ARENAHEIGHT)
			return 1;
		return 0;
	} else {
		if ((ypos < -(spr->width)))
			return -1;
		if (ypos > ARENAHEIGHT)
			return 1;
		return 0;
	}
}

void Thing::keepInArena() {
	if (xpos > (ARENAWIDTH - spr->width))
		xpos = (ARENAWIDTH - spr->width);
	if (xpos < 0)
		xpos = 0;
	if (ypos > (ARENAHEIGHT - spr->height))
		ypos = (ARENAHEIGHT - spr->height);
	if (ypos < 0)
		ypos = 0;
}

void Thing::bounce() {
	if (XoutofArena() == 1)		xposdir = -abs(xposdir); // ensure negative
	if (XoutofArena() == -1)	xposdir = abs(xposdir);
	if (YoutofArena() == 1)		yposdir = -abs(yposdir);
	if (YoutofArena() == -1)	yposdir = abs(yposdir);
	keepInArena();
}

int Thing::quantiseDirection(int dir) {
	if (dir > 128)		return 256;
	else if (dir < -128) 	return -256;
	else			return 0;
}

bool Thing::colcheck(Thing* thingb) {
	// don't colcheck things in midair
	if ((zvelocity != 0) || (thingb->zvelocity != 0) || (zpos > 1000))
		return false;
	
	int size = worldobj->monster_size << 8;
	if (size) {	
		// in shrink mode
		if (	(this->xpos 	+ size) 	> thingb->xpos &&
			(thingb->xpos 	+ size) 	> this->xpos &&
			
			(this->ypos 	+ size) 	> thingb->ypos &&
			(thingb->ypos 	+ size) 	> this->ypos )
		{
			return true; // don't bother with pixel perfect stuff while shrunken
		} else {
			return false;
		}
	} else {
		// Is there a bounding box overlap ??
		Sprite* sprb = thingb->spr;
		if (	(this->xpos 	+ spr->width) 	> thingb->xpos &&
			(thingb->xpos 	+ sprb->width) 	> this->xpos &&
			
			(this->ypos + spr->height) > thingb->ypos &&
			(thingb->ypos + sprb->height) > this->ypos )
		{
			return (spr->andcheck(xpos, ypos, frame, facingleft,
					sprb, thingb->xpos, thingb->ypos,
					thingb->frame, thingb->facingleft));
		} else {
			return false;
		}
	}
}
