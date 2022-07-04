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

#ifndef THING_HH
#define THING_HH

#include "Globals.hh"

class Sprite;

class Thing {

protected:

	double circletime;
	int wobbleamplitude, wobblespeed;

	int lookremainder, frameremainder;
	static int recoildistance;
	int gap;

	// TODO: put these in Sprite
	int animdirection;
	
	// set timebetweenlook for infrequent direction changes
	void chase(int delta, int timebetweenlook=0, Thing* target=0);
	void seekWalls(int delta);
	void randomMove(int delta, int timebetweenlook=0);
	int quantiseDirection(int dir);
	void normaliseDirection();
	void XCosMove(int amplitude=0); // 0 uses wobbleamplitude
	void YSinMove(int amplitude=0);
	void wobbleChase(int delta, bool vertwobble=true);
	void centerChase(int delta);
	
	// if arg is false, triggers when sprite is out of sight
	int XoutofArena(bool includespr = true);
	int YoutofArena(bool includespr = true);

	void keepInArena();
	void bounce();
	void movez(int delta);

	void explode(int size, Globals::sprindex type);
	void bonusGen(int x, int y, int luck);

	virtual int getTimePerFrame();

public:
	enum type { ENEMY, PLAYER, OBSTACLE, NONE }; 
	
	int xpos, ypos, zpos; // NB. These are in 1/256 pixels. :-P
	unsigned int frame;
	bool facingleft, animpingpong, hasfacing;

	int xwobblecent, ywobblecent;
	int health, zvelocity, speed;
	
	int xposdir, yposdir;
	double invhypot;
	
	bool optionalkill; // only relevant for type ENEMY
	bool killsplayerbullet; // likewise
	bool dropsbarricade;
	bool levchangepersistant;
	Thing* next;
	Thing* prev; // only relevant for linked list creatures, like Centipedes
	
	Sprite* spr;
	Globals::sprindex sprnumber;
	void setSprite(Globals::sprindex newspr);
	void setupAnim();
	
	Thing();
	Thing(Globals::sprindex _spr);
	virtual ~Thing();
	
	virtual void draw();
	virtual void move(int delta);
	virtual void jump(int velocity);
	virtual bool colcheck(Thing* thingb);
	
	virtual int getSpeed();
	int getXCenter();
	int getYCenter();
	void randomPos();
	virtual void startPos();
	virtual void getInBehind() { }	// for linked list enemies
	virtual void recoil(int injury, int xdir, int ydir);
	virtual void pull(int delta, int xd, int yd);
	virtual void detach(Thing* t);
	virtual void hurt(int injury, int xdir=0, int ydir=0);
	
	virtual Thing::type getType() 	{ return Thing::NONE; }
	virtual void tookEnemy(Thing* enemy) {}
	virtual void tookPlayer(Thing* play) {}
	virtual void tookObstacle(Thing* obst);
	virtual bool listensEnemy() 	{ return false; }
	virtual bool listensPlayer() 	{ return false; }
	virtual bool listensObstacle() 	{ return true; }
};

#endif
