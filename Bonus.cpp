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

/* Upon adding bonus remember to:
	Add references to a new global sprite
	Give it an entry in the prob enum (see Bonus.hh)
	Initialise its probability in initProb()
	Write instantiating code in give()
*/

#include <cmath>
#include <iostream>
#include <SDL.h>
#include "Bonus.hh"
#include "Player.hh"
#include "Sprite.hh"
#include "World.hh"
#include "MiscDisplay.hh"
#include "Bullet.hh"
#include "Barricade.hh"

using namespace std;
extern World* worldobj;
extern Globals* globals;

int Bonus::prob[BONUSEND];
int Bonus::maxprob = 0;

Bonus::Bonus(Globals::sprindex _spr) : Thing(_spr) {
	health = globals->loadInt("Constants/bonus_timeout");
	owner = NULL;
	
	switchedon = false;
	optionalkill = true; // yeah, but if the warp has already timed out, might as well let them have it
	xmov = (int)(24 * ((float) rand() / RAND_MAX)) - 12; 
	name = "Generic Bonus";
}

Bonus::~Bonus() {
}

void Bonus::initProb() {
	maxprob = globals->loadInt("Bonus/maxchances");
	prob[XLIFE] = globals->loadInt("Bonus/xlifechance");
	prob[SHADES] = globals->loadInt("Bonus/shadeschance");
	prob[SHIELD] = globals->loadInt("Bonus/shieldchance");
	prob[CRATES] = globals->loadInt("Bonus/cratechance");
	prob[THREEWAY] = globals->loadInt("Bonus/threewaychance");
	prob[REPEL] = globals->loadInt("Bonus/repelchance");
	prob[SLOW] = globals->loadInt("Bonus/slowchance");
	prob[DROID] = globals->loadInt("Bonus/circledroidchance");
	prob[RAM] = globals->loadInt("Bonus/ramdroidchance");
	prob[FLAME] = globals->loadInt("Bonus/flamechance");
	prob[MACHINE] = globals->loadInt("Bonus/machinegunchance");
	prob[SHRINK] = globals->loadInt("Bonus/shrinkchance");
	prob[SPEED] = globals->loadInt("Bonus/speedchance");
	prob[XTIME] = globals->loadInt("Bonus/xtimechance");
	prob[REFLECT] = globals->loadInt("Bonus/reflectchance");
	prob[MINES] = globals->loadInt("Bonus/mineschance");
	prob[TELEPORT] = globals->loadInt("Bonus/telepchance");
	prob[BOMB] = globals->loadInt("Bonus/bombchance");
	prob[REARFIRE] = globals->loadInt("Bonus/rearfirechance");

}

// this sucks
void Bonus::give(int type, int x, int y) {
	Bonus* newbon;
	switch(type) {
		// can turn some of these into boolswitchers now
		case XLIFE:  newbon = new Xlife(x, y); break; 
		case SHADES:  newbon = new Shades(x, y); break; 
		case SHIELD:  newbon = new Shield(x, y); break; 
		case CRATES:  newbon = new Instacrate(x, y); break; 
		case THREEWAY:  newbon = new ThreeWay(x, y); break; 
		case REPEL:  newbon = new MonstSpeed(x, y, -0.5f); break; 
		case SLOW:  newbon = new MonstSpeed(x, y, 0.3f); break; 
		case DROID:  newbon = new DroidPickup(x, y); break; 
		case RAM:  newbon = new RamPickup(x, y); break; 
		case FLAME:  newbon = new Flame(x, y); break; 
		case MACHINE:  newbon = new MachineGun(x, y); break; 
		case SHRINK:  newbon = new Shrink(x, y); break; 
		case SPEED:  newbon = new SpeedUp(x, y); break; 
		case XTIME:  newbon = new Xtime(x, y); break; 
		case REFLECT:  newbon = new Reflection(x, y); break; 
		case MINES:  newbon = new Mines(x, y); break; 
		case TELEPORT:  newbon = new Teleport(x, y); break; 
		case BOMB:  newbon = new Bomb(x, y); break; 
		case REARFIRE:  newbon = new RearFire(x, y); break; 
		default:  cout << "Bonus::give   Warning! Bonus type " << type << " not found" << endl;
			newbon = 0;
	}
	if (newbon) {
		worldobj->addThing(newbon);
		newbon->jump(globals->loadInt("Constants/fruitbonusbounce"));
	}
}

/* luck value is 0-10, 	from 0 which is no extra luck
			to 10 which is guaranteed extra life */
void Bonus::randGen(int luck, int x, int y) {
	int roll=(int)((globals->loadInt("Bonus/maxchances") * (10 - luck))/10 * 
		((float)rand() / RAND_MAX));

	for (int i = 1; i < BONUSEND; i++) {
		if ((roll -= prob[i]) < 0) {
			give(i, x, y);
			return;
		}
	}
}


void Bonus::activated() {
	health -= worldobj->delta;
}

void Bonus::move(int delta) {
	health -= delta;  
	if (zpos == 0) {
		chase(delta);
	} else {
		xpos += xmov * delta;
	}
	Thing::move(delta);
}

void Bonus::draw() {
	Thing::draw();

	bool drawbubble = (health > 1000) || (frame == 1);
	if (drawbubble) {
		Sprite* bubble = globals->spr[Globals::BONUSBUBBLE];

		int centering = bubble->width/2 - spr->width/2;
		int bubframe = 0;
		if (frame <= bubble->getFrames())	bubframe = frame;

		bubble->draw(xpos - centering, ypos - centering, bubframe, false);
	}
}

int Bonus::getSpeed()	{ return globals->loadInt("Constants/bonusspeed"); }

void Bonus::tookPlayer(Thing* play) {
	Player* p = static_cast<Player *>(play);

	p->addToInventory(this);
	worldobj->miscdisplay->setConsole(name, globals->loadInt("Constants/bonusinfotimeout"));
}

void Bonus::startPos() {
	health = 0;
}


Fruit::Fruit(bool colours[6]) : Thing(Globals::BONUSFRUIT) {
	// random selection here
	int onescolumn = (unsigned int)(3 * ((float)rand() / RAND_MAX)) + 1;
	int tenscolumn;

	// O(random) algorithm. :P
	do {
		tenscolumn = (unsigned int)(6 * ((float)rand() / RAND_MAX));
	} while(colours[tenscolumn] == false);

	frame = onescolumn + 10*tenscolumn;
	myflav = (Fruit::flavour)(onescolumn + 10*tenscolumn);
	if (globals->verbosity > 0) 	cout << "New fruit: " << (int)myflav << endl;

	health = 100;
	optionalkill = true;
	blinking = false;
	maxage = worldobj->warp_time*1000 + globals->loadInt("Constants/minfruitrottime") +
		(int)(globals->loadInt("Constants/fruitrottimevariation") * ((float)rand() / RAND_MAX));
}

Fruit::~Fruit() {}

void Fruit::tookPlayer(Thing* play) {
	if (health) {	// might already have been taken this frame
		// if not, give to player
		Player* owner = static_cast<Player *>(play);
		owner->addToFruit(myflav);
		health = 0;
	}
}

void Fruit::move(int delta) {
	if (worldobj->getLevelAge() > maxage) {
		health = 0;
	} else if (worldobj->getLevelAge() > maxage - 3000) {
		blinking = true;
	}
}

void Fruit::draw() {
	if (!blinking || (SDL_GetTicks()%400) > 200)
		Thing::draw();
}


Bomb::Bomb(int x, int y) : Bonus(Globals::BONUSBOMB) {
	xpos = x; ypos = y;
	name = "Bomb";
}

void Bomb::activated() {
	Thing* newbomb = new ThrownBomb(Globals::BOMB, owner->xpos, owner->ypos, 
					owner->xaim*256, owner->yaim*256);
	worldobj->addThing(newbomb);
	health = 0;
}


Xlife::Xlife(int x, int y) : Bonus(Globals::BONUSXLIFE) {
	xpos = x; ypos = y;
	// don't go to next level if it's last on the screen
	optionalkill = false;
	name = "Extra Life";
}
	
void Xlife::tookPlayer(Thing* play) {
	worldobj->lives++;
	Player* taker = static_cast<Player *>(play);
	taker->stats->tookXlife();
	health = 0;
}


Xtime::Xtime(int x, int y) : Bonus(Globals::BONUSXTIME) {
	xpos = x; ypos = y;
	// don't go to next level if it's last on the screen
	optionalkill = false;
	name = "Extra 5 seconds warp time";
}
	
void Xtime::tookPlayer(Thing* play) {
	worldobj->warp_time += globals->loadInt("Constants/timebonus");
	health = 0;
}


Teleport::Teleport(int x, int y) : Bonus(Globals::BONUSTELEPORT) {
	xpos = x; ypos = y;
	name = "Teleport";
}
	
void Teleport::activated() {
	if (health == globals->loadInt("Constants/bonuslength")) {
		// first time called
		health = 1000;
		owner->randomPos();
	}
	Bonus::activated();
	owner->invincible = true;
}

void Teleport::deactivated() {
	owner->invincible = false;
}

SpeedUp::SpeedUp(int x, int y) : Bonus(Globals::BONUSSPEEDUP) {
	xpos = x; ypos = y;
	name = "Speed Boost";
}

void SpeedUp::activated() {
	Bonus::activated();
	owner->speed = globals->loadInt("PlayerSpecs/fastplayerspeed");
}

void SpeedUp::deactivated() {
	owner->speed = globals->loadInt("PlayerSpecs/playerspeed");
}


Shrink::Shrink(int x, int y) : Bonus(Globals::BONUSSHRINK) {
	xpos = x; ypos = y;
	name = "Monster Shrink";
}

void Shrink::activated() {
	Bonus::activated();
	worldobj->monster_size = globals->loadInt("Constants/shrunkenmonstersize");
}

void Shrink::deactivated() {
	worldobj->monster_size = 0;
}


ThreeWay::ThreeWay(int x, int y) : Bonus(Globals::BONUSTHREEWAY) {
	xpos = x; ypos = y;
	name = "Three way fire";
}

void ThreeWay::activated() {
	Bonus::activated();
	owner->threewayon = true;
}

void ThreeWay::deactivated() {
	owner->threewayon = false;
}


RearFire::RearFire(int x, int y) : Bonus(Globals::BONUSREARFIRE) {
	xpos = x; ypos = y;
	name = "Rear fire";
}

void RearFire::activated() {
	Bonus::activated();
	owner->rearfireon = true;
}

void RearFire::deactivated() {
	owner->rearfireon = false;
}


Flame::Flame(int x, int y) : Bonus(Globals::BONUSFLAME) {
	xpos = x; ypos = y;
	name = "Flamethrower";
}

void Flame::activated() {
	Bonus::activated();
	owner->flamingon = true;
}

void Flame::deactivated() {
	owner->flamingon = false;
}


Reflection::Reflection(int x, int y) : Bonus(Globals::BONUSREFLECTION) {
	xpos = x; ypos = y;
	name = "Reflective Bullets";
}

void Reflection::tookPlayer(Thing* x) {
	Bonus::tookPlayer(x);
	health = globals->loadInt("Bonus/reflecttime");
}

void Reflection::activated() {
	Bonus::activated();
	owner->reflectingon = true;
}

void Reflection::deactivated() {
	owner->reflectingon = false;
}


Shield::Shield(int x, int y) : Bonus(Globals::BONUSSHIELD) {
	xpos = x; ypos = y;
	name = "Invincibility";
}

void Shield::activated() {
	Bonus::activated();
	owner->invincible = true;
}

void Shield::deactivated() {
	owner->invincible = false;
}


// adds to the game's object load by about 20
Mines::Mines(int x, int y) : Bonus(Globals::BONUSMINES) {
	xpos = x; ypos = y;
	name = "Mines";
}

void Mines::activated() {
	Bonus::activated();
	owner->mineson = true;
}

void Mines::deactivated() {
	owner->mineson = false;
}

MonstSpeed::MonstSpeed(int x, int y, float _multiplier) : Bonus(Globals::BONUSSLOW) {
	xpos = x; ypos = y;
	multiplier = _multiplier;
	
	if (multiplier == 0) {
		setSprite(Globals::BONUSFREEZE); 
		name = "Monster Freeze";
	} else if (multiplier < 0) {
		setSprite(Globals::BONUSREPEL);
		name = "Monster Repellent";
	} else {
		setSprite(Globals::BONUSSLOW);
		name = "Monster Slowdown";
	}
}

void MonstSpeed::activated() {
	Bonus::activated();
	worldobj->monster_speed = multiplier;
}

void MonstSpeed::deactivated() {
	worldobj->monster_speed = (float)globals->loadInt("Constants/monsterspeed")/100;
}


DroidPickup::DroidPickup(int x, int y) : Bonus(Globals::BONUSDROID) {
	xpos = x; ypos = y; 
	dra = 0; drb = 0;
	name = "Circling Droid";
}

void DroidPickup::tookPlayer(Thing* x) {
	Bonus::tookPlayer(x);
	health = globals->loadInt("Bonus/circletime");
}

void DroidPickup::activated() {
	Bonus::activated();
	if (!(dra || drb)) {
		dra = new ActiveDroid(owner, 0);
		drb = new ActiveDroid(owner, M_PI);
		worldobj->addThing(dra);
		worldobj->addThing(drb);
	}
}

void DroidPickup::deactivated() {
	dra->health = 0; // World will delete these
	drb->health = 0;
}


RamPickup::RamPickup(int x, int y) : Bonus(Globals::BONUSDROID) {
	xpos = x; ypos = y; 
	dr = 0;
	name = "Ram Droid"; // quadralien forever
}

void RamPickup::tookPlayer(Thing* x) {
	Bonus::tookPlayer(x);
	health = globals->loadInt("Bonus/ramtime");
}

void RamPickup::activated() {
	Bonus::activated();
	if (!dr) {
		dr = new RamDroid(owner);
		worldobj->addThing(dr);
	}
}

void RamPickup::deactivated() {
	dr->health = 0;
}


MachineGun::MachineGun(int x, int y) : Bonus(Globals::BONUSMACHINEGUN) {
	xpos = x; ypos = y;
	name = "Machine Gun";
}

void MachineGun::tookPlayer(Thing* x) {
	Bonus::tookPlayer(x);
	health = globals->loadInt("Bonus/machinetime");
}

void MachineGun::activated() {
	Bonus::activated();
	owner->machinegunon = true;
	owner->reloadtime = globals->loadInt("PlayerSpecs/fastreloadtime");
}

void MachineGun::deactivated() {
	owner->machinegunon = false;
	owner->reloadtime = globals->loadInt("PlayerSpecs/reloadtime");
}


Instacrate::Instacrate(int x, int y) : Bonus(Globals::BONUSCRATE) {
	xpos = x; ypos = y;
	name = "InstaCrate!";
}

void Instacrate::placeCrate(int x, int y) {
	Thing* c = new Barricade(Globals::CRATE, x, y);
	Thing* ci = new BarricadeImpact(x, y);
	worldobj->addThing(c);
	worldobj->addThing(ci);
}

void Instacrate::activated() {
	int d = globals->loadInt("Constants/instacratedistance");

	// for each corner of a square around the player
	for (int yy=-1; yy <= 1; yy+=2) {
		for (int xx=-1; xx <= 1; xx+=2) {
			int xp = owner->xpos+(xx*d/2);
			int yp = owner->ypos+(yy*d/2);
			xp += 600; yp += 600; // kludge to get bullets lined up
			int threequarterd = d*3 / 4;

			placeCrate(xp + xx*threequarterd, yp);
			placeCrate(xp, yp + yy*threequarterd);
		}
	}
	health = 0;
}


Shades::Shades(int x, int y) : Bonus(Globals::BONUSSHADES) {
	xpos = x; ypos = y;
	name = "The Bitmap Brothers Tribute Bonus";
}

void Shades::tookPlayer(Thing* x) {
	Bonus::tookPlayer(x);
	health = globals->loadInt("Bonus/shadestime");
}

void Shades::activated() {
	Bonus::activated();
	owner->threewayon = 
	owner->mineson = 
	owner->flamingon = 
	owner->reflectingon = 	
	owner->rearfireon =
	true;
}

void Shades::deactivated() {
	owner->threewayon = 
	owner->mineson = 
	owner->flamingon = 
	owner->reflectingon = 	
	owner->rearfireon = 
	false;
}


ActiveDroid::ActiveDroid(Player* _owner, double position) : Thing(Globals::DROID) {
	owner = _owner;
	wobbleamplitude = globals->loadInt("Constants/droidradius");
	levchangepersistant = optionalkill = true;
	circletime = position;
	killsplayerbullet = false;
}

void ActiveDroid::tookEnemy(Thing* x) {
	x->hurt(worldobj->delta*4);
	x->recoil(worldobj->delta/2, owner->xaim, owner->yaim);
	circletime -= 0.3;
}

void ActiveDroid::move(int delta) {
	xwobblecent = owner->xpos;
	ywobblecent = owner->ypos;
	YSinMove();	XCosMove();
	circletime += (worldobj->monster_speed * delta * 
		globals->loadInt("Constants/droidspeed"))/5000;
}

void ActiveDroid::startPos() {
	move(0);
}


RamDroid::RamDroid(Player* _owner) : Thing(Globals::DROID) {
	owner = _owner;
	weardistance = globals->loadInt("Constants/droidweardistance");
	move(0);
	levchangepersistant = optionalkill = true;
	killsplayerbullet = false;
}

void RamDroid::tookEnemy(Thing* x) {
	x->hurt(worldobj->delta*4);
	x->recoil(worldobj->delta/2, owner->xaim, owner->yaim);
}

void RamDroid::move(int delta) {
	xpos = owner->xpos + owner->xaim*weardistance;
	ypos = owner->ypos + owner->yaim*weardistance;
}
