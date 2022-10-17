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

#ifndef PLAYER_HH
#define PLAYER_HH

#include "Thing.hh"
#include "InputState.hh"
#include "ScoreBoard.hh"
#include "PlayerStats.hh"
#include "Bonus.hh"

#define MAXINV 4

class Player : public Thing {
public:
	Player(int _playernumber, bool demo);
	~Player();

	int playernumber;
	InputState* in;
	ScoreBoard* scoreboard;
	PlayerStats* stats;
	Bonus* inventory[MAXINV]; // index 0 is the top one shown to screen.
	Fruit::flavour taken[3];
	Sprite *arm;
	Sprite *warp1;
	Sprite *warp1arm;
	Sprite *warp2;
	Sprite *warp2arm;
	
	void move(int delta);
	void warp(int delta);
	int getSpeed();
	void speedUp(int incs);
	void startPos();
	void draw();
	void hurt(int injury, int xdir=0, int ydir=0);
	void addToInventory(Bonus* newb);
	void addToFruit(Fruit::flavour newfruit);

	Thing::type getType() { return Thing::PLAYER; }

	bool threewayon, mineson, reflectingon, flamingon, invincible, rearfireon, machinegunon;
	bool playing;
	int bonusstate;
	int reloadtime, firepower;
	int xdir, ydir; // direction facing
	int xaim, yaim;	// direction firing
	bool warping;
	int warpxpos;

	private:
	int remainder;
	int usedelay;
	int warpdistance;
	bool forcefieldflicker;

	int momentum;
	unsigned int framecolumn;

	void fire(int _xdir, int _ydir);
	void mine();
};
 
#endif
