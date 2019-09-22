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

#ifndef BONUS_HH
#define BONUS_HH

#include "Thing.hh"

class Player;

class Bonus : public Thing {
public:
	enum id { 	NONE, XLIFE, SHADES, SHIELD, CRATES, THREEWAY, 
			REPEL, SLOW, DROID, RAM, FLAME, MACHINE, SHRINK,
			SPEED, XTIME, REFLECT, MINES, TELEPORT, BOMB, 
			REARFIRE, BONUSEND
	};

	static void give(int type, int x, int y); 
	static void randGen(int luck, int x, int y); 
	static void initProb(); 
	static int prob[BONUSEND];
	static int maxprob;

	Player* owner;
	bool switchedon;
	int xmov;	// mess with this for a bit of variation when kicking out multiple bonuses due to a fruit combo
	Bonus(Globals::sprindex _spr);
	virtual ~Bonus();
	virtual void move(int delta);
	virtual void draw();
	virtual bool listensPlayer()	{ return true; }
	virtual bool listensObstacle()	{ return false; }
	virtual void tookPlayer(Thing* play);
	virtual void activated();
	virtual void deactivated() {}
	void startPos();

protected:
	std::string name;
	virtual int getSpeed();
};


class Bomb : public Bonus {
public:
	Bomb(int x, int y);
	void activated();
};

class Instacrate : public Bonus {
public:
	Instacrate(int x, int y);
	void activated();
private:
	void placeCrate(int x, int y);
};

class Xlife : public Bonus {
public:
	Xlife(int x, int y);
	void tookPlayer(Thing* play);
};

class Xtime : public Bonus {
public:
	Xtime(int x, int y);
	void tookPlayer(Thing* play);
};

class Teleport : public Bonus {
public:
	Teleport(int x, int y);
	void activated();
	void deactivated();
};

// not much in common, won't inherit Bonus
class Fruit : public Thing {
public:
	enum flavour {	NONE=99, WILDCARD=1,
			STRAWBERRY=11, REDAPPLE, CHERRY, 
			ORANGE=21, APRICOT, PEACH,
			LEMON=31, PINEAPPLE, BANANA,
			MELON=41, GREENGRAPES, GREENAPPLE,
			BLUEBERRY=51, PLUM, BLUEGRAPES
			};
	Fruit(bool colours[6]);
	~Fruit();
	Fruit::flavour myflav;

	bool listensPlayer() { return true; }
	bool listensObstacle() { return false; }
	void tookPlayer(Thing* play);
	void move(int delta);
	void draw();
	int maxage;
	bool blinking;
};

class SpeedUp : public Bonus {
public:
	SpeedUp(int x, int y);
	void activated();
	void deactivated();
};

class Shrink : public Bonus {
public:
	Shrink(int x, int y);
	void activated();
	void deactivated();
};

class ThreeWay : public Bonus {
public:
	ThreeWay(int x, int y);
	~ThreeWay() {}
	void activated();
	void deactivated();
};

class RearFire : public Bonus {
public:
	RearFire(int x, int y);
	~RearFire() {}
	void activated();
	void deactivated();
};

class Flame : public Bonus {
public:
	Flame(int x, int y);
	~Flame() {}
	void activated();
	void deactivated();
};

class Reflection : public Bonus {
public:
	Reflection(int x, int y);
	~Reflection() {}
	void tookPlayer(Thing* x);
	void activated();
	void deactivated();
};

class Shield : public Bonus {
public:
	Shield(int x, int y);
	~Shield() {}
	void activated();
	void deactivated();
};

class Mines : public Bonus {
public:
	Mines(int x, int y);
	~Mines() {}
	void activated();
	void deactivated();
};

class MonstSpeed : public Bonus {
public:
	MonstSpeed(int x, int y, float _multiplier);
	~MonstSpeed() {}
	void activated();
	void deactivated();
	float multiplier;
};


class ActiveDroid : public Thing {
public:
	ActiveDroid(Player* _owner, double position);
	Thing::type getType()	{ return Thing::OBSTACLE; }
	bool listensEnemy() { return true; }
	void tookEnemy(Thing* x);
	void move(int delta);
	void startPos();
	Player* owner;
};

class RamDroid : public Thing {
public:
	RamDroid(Player* _owner);
	Thing::type getType()	{ return Thing::OBSTACLE; }
	bool listensEnemy() { return true; }
	void tookEnemy(Thing* x);
	void move(int delta);
	Player* owner;
	int weardistance;
};

class DroidPickup : public Bonus {
public:
	DroidPickup(int x, int y);
	~DroidPickup() {}
	void tookPlayer(Thing* x);
	void activated();
	void deactivated();
	ActiveDroid* dra;
	ActiveDroid* drb;
};

class RamPickup : public Bonus {
public:
	RamPickup(int x, int y);
	~RamPickup() {}
	void tookPlayer(Thing* x);
	void activated();
	void deactivated();
	RamDroid* dr;
};


class MachineGun : public Bonus {
public:
	MachineGun(int x, int y);
	~MachineGun() {}
	void tookPlayer(Thing* x);
	void activated();
	void deactivated();
};

// the Bitmap Brothers' tribute bonus. :)
class Shades : public Bonus {
public:
	Shades(int x, int y);
	~Shades() {}
	void tookPlayer(Thing* x);
	void activated();
	void deactivated();
};

#endif
