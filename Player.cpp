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

#include <stdio.h>
#include <iostream>
#include <SDL.h>
#include "Bullet.hh"
#include "Player.hh"
#include "World.hh"
#include "Sprite.hh"
#include "Bonus.hh"
#include "Sound.hh"

using namespace std;
extern World* worldobj;
extern Globals* globals;

Player::Player(int _playernumber) : Thing((Globals::sprindex)_playernumber) {
	playernumber = _playernumber;
	arm = globals->spr[Globals::MONKEYARMONE + _playernumber];

	//in = new InputState(playernumber);
	in = InputState::playercontrols[playernumber];
	scoreboard = new ScoreBoard(this);
	stats = new PlayerStats(this);
	
	inventory[0] = NULL; inventory[1] = NULL;
	inventory[2] = NULL; inventory[3] = NULL;
	taken[0] = taken[1] = taken[2] = Fruit::NONE;
	bonusstate = 0;
	
	levchangepersistant = optionalkill = true;

	remainder = 0;
	usedelay = 0;
	xdir = -1; ydir = 0;
	xaim = -1; yaim = 0;
	momentum = 0;
	
	//lives = globals->loadInt("PlayerSpecs/startlives");
	speed = globals->loadInt("PlayerSpecs/playerspeed");
	reloadtime = globals->loadInt("PlayerSpecs/reloadtime");
	firepower = globals->loadInt("PlayerSpecs/firepower");

	machinegunon = rearfireon = invincible = threewayon = mineson = reflectingon = flamingon = false;
	startPos();
}

Player::~Player() {
	delete scoreboard;
	if (globals->verbosity > 0)
		cout << "Player deleted" << endl;
}

void Player::startPos() {
	xpos = ARENAWIDTH/2;
	ypos = ARENAHEIGHT/2;

	// back to starting health, bit dodgy to have this here
	health = globals->loadInt("PlayerSpecs/health");

	/* an attempt to have players start from different places 
	//xpos = (playernumber+1)*10000 + ARENAWIDTH/2;*/
}

int Player::getSpeed() {
	return (speed + (momentum>>8));
}

void Player::speedUp(int amount) {
	speed = globals->loadInt("PlayerSpecs/playerspeed") +
		amount * globals->loadInt("PlayerSpecs/speedinc");
}

void Player::hurt(int injury, int xdir, int ydir) {
	if (health < 1) {
		// dead already, ignore it.
		return;
	}
	
	if (invincible) {
		recoil(injury, xdir, ydir);
		worldobj->flashing = 40;
	} else {
		if (injury >= health) {
			health -= injury;
			stats->died();
			worldobj->lives--;

			if (worldobj->lives < 0) {
				Sound::playSound("gameover");
				InputState::anyKeyPress();
				worldobj->gameDelay(	globals->loadInt("Constants/deathpause"), 
							World::DELAY, World::GAMEOVER);
				stats->totaltime += worldobj->getLevelAge()/1000;
			} else {
				Sound::playSound("die");
				worldobj->gameDelay(	globals->loadInt("Constants/deathpause"), 
							World::DELAY, World::DIED);
			}
		} else {
			Thing::hurt(injury);
		}
	}
}

void Player::fire(int _xdir, int _ydir) {
	Sound::playSound("fire");
	
	Bullet* newbull;
	Globals::sprindex bulletspr;
	int type;
	type = (int)reflectingon;
	type += 2*((int)flamingon);
	if (!(flamingon || machinegunon))
		type += 4; // orientated
		
	if (flamingon)
		bulletspr = Globals::FLAME;
	else if (machinegunon)
		bulletspr = Globals::PELLET;
	else
		bulletspr = Globals::BULLET;
	
	int bulletxpos = xpos;
	int bulletypos = ypos;
	if (machinegunon) {
		// add spread
		_xdir += (int)((40.0*rand()/RAND_MAX) - 20);
		_ydir += (int)((40.0*rand()/RAND_MAX) - 20);
		// bullets not player sized, 
		// so will be in topleft corner otherwise
		bulletxpos = xpos + spr->width/2;
		bulletypos = ypos + spr->height/2;
	}

	newbull = new Bullet(	bulletspr, type, bulletxpos, bulletypos, _xdir, _ydir, this );
	worldobj->addThing(newbull);
}

void Player::mine() {
	// 4 is orientated. not animating
	Thing* newbull = new Bullet(	Globals::BULLET, 4, xpos, ypos,
						0, 0, this );
	worldobj->addThing(newbull);
}

void Player::addToInventory(Bonus* newb) {
	if (newb->owner) {	// someone's already got this one
		return;
	} else {
		if (globals->verbosity > 0) 	cout << "Item " << this << " entering inventory" << endl;
		newb->owner = this;
		newb->health = globals->loadInt("Constants/bonuslength");
		worldobj->removeThing(newb); // take it off the all list, don't kill it
	}

	Sound::playSound("takebonus");
	
	stats->tookPowerup();
	scoreboard->highlightInventory();

	if (inventory[0] == NULL) {
		inventory[0] = newb;
		if (globals->verbosity > 0)
			cout << "0" << ": inventory item " << inventory[0] << endl;
	} else {
		int i = 1;
		while (i < (MAXINV-1) && inventory[i] != NULL) {
			i++;
		}
		
		if (inventory[i] != 0) {
			if (inventory[i]->switchedon) inventory[i]->deactivated();
		}
		inventory[i] = inventory[0];
		inventory[0] = newb;
	
		if (globals->verbosity > 0) {
			cout << "Freed slot 0" << endl;
			cout << "0" << ": inventory item " << inventory[0] << endl;
			cout << i << ": inventory item " << inventory[i] << endl;
		}
	}
}

// relies on exceptions, may be slow
void Player::addToFruit(Fruit::flavour newfruit) {
	Sound::playSound("tookfruit");
	
	taken[2] = taken[1]; taken[1] = taken[0];
	taken[0] = newfruit;
	if (globals->verbosity > 0)
		cout << "Fruit: " << (int)taken[0] << (int)taken[1] << (int)taken[2] << endl;

	char resstring[19];
	char howmany[30];
	sprintf(resstring, "FruitCombos/%d%d%d", 
		(int)taken[0], (int)taken[1], (int)taken[2]);
	sprintf(howmany, "FruitCombos/%d%d%d-multiplier", 
		(int)taken[0], (int)taken[1], (int)taken[2]);

	// fruit specific
	int bonusa = globals->loadInt(resstring);
	if (bonusa) {
		int count = globals->loadInt(howmany);
		if (count == 0) count = 1;
		for(int i=0; i < count; i++) {
			Bonus::give(bonusa, xpos, ypos);
		}
		Sound::playSound("combo");
	}

	// colour specific. (not bothering with multipliers here)
	resstring[17]=resstring[15]=resstring[13]='0';

	if (globals->verbosity > 0)
		cout << resstring << endl;
	int bonusb = globals->loadInt(resstring);
	if (bonusb) {
		Bonus::give(bonusb, xpos, ypos);
		Sound::playSound("combo");
	}
	
	if (bonusa || bonusb) {
		scoreboard->highlightFruit();
		// make it a little less easy to have massive runs of fruit
		taken[2] = taken[1] = Fruit::NONE;
	}
	stats->tookFruit();
}

void Player::draw() {
	Thing::draw();
	int armframe = 0;
	if (xaim != 0)
	{
		if (yaim == 0)
			armframe = 2;
		else if (yaim < 0)
			armframe = 1;
		else
			armframe = 3;
	}
	else if (yaim == 0)
		armframe = 3;
	else if (yaim > 0)
		armframe = 4;
	arm->draw(xpos, ypos, armframe, facingleft);
	if (invincible) {
		if ((SDL_GetTicks() % 100) > 50)
			globals->spr[Globals::FORCEFIELD]->draw(xpos, ypos, 0, false);
	}
}

void Player::move(int delta) {
	Thing::move(delta);

	// iterate through calling the methods on activated bonuses
	for(int i=0; i < MAXINV && inventory[i] != NULL; i++) {
	
		if (inventory[i]->switchedon) {
			inventory[i]->activated();
			
			if (inventory[i]->health <= 0) {
				Sound::playSound("deactivated");
				// if the any bonuses have timed out
				inventory[i]->deactivated();
				delete inventory[i];
				inventory[i] = NULL;
			
				// move every thing down one slot
				for(int j=i; j < (MAXINV-1); j++) {
					inventory[j] = inventory[j+1];

					if (globals->verbosity > 0)
						cout << j << ": inventory item " << inventory[j] << endl;
				}
				inventory[MAXINV-1] = NULL;
				if (globals->verbosity > 0)
					cout << MAXINV-1 << ": inventory item " << inventory[MAXINV-1] << endl;
			}
		}
	}
	usedelay += delta;
	if (in->use->is_pressed() && inventory[0] && usedelay > 500) {
		Sound::playSound("activated");
		usedelay = 0;
		inventory[0]->switchedon = true;
	}

	int inx = (int)(in->movex->get_pos());
	int iny = (int)(in->movey->get_pos());
	int finx = (int)(in->firex->get_pos());
	int finy = (int)(in->firey->get_pos());

	if (inx || iny) {
		// if move keys are pressed, face in a new direction
		xposdir = inx;	yposdir = iny;
	}

	if (finx || finy) {
		// if aim keys are pressed, set a new firing direction
		xaim = finx;	yaim = finy;
	}

	if (xaim)
		// face in the direction we're firing
		xposdir = xaim;

	remainder += delta;
	if (remainder > reloadtime) {
		remainder -= reloadtime;
		
		fire(xaim*256, yaim*256);

		if (threewayon) {
			if (xaim && yaim) { 
				fire(0, yaim*256); fire(xaim*256, 0); 
			} else if (xaim) {
				fire(xaim*256, 256); fire(xaim*256, -256);
			} else {
				fire(256, yaim*256); fire(-256, yaim*256);
			}
		}

		if (rearfireon) {
			fire(xaim*-256, yaim*-256);
		}

		if (mineson) {
			mine();
		}

		if (finx || finy) {
			xaim = finx;
			yaim = finy;
		}
	}

	// finally, move the player
	xpos += (inx*getSpeed()*delta);
	ypos += (iny*getSpeed()*delta);
	
	keepInArena();
} 
