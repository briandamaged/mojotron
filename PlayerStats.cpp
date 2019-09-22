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
#include "PlayerStats.hh"
#include "Player.hh"

using namespace std;
extern Globals* globals;

int PlayerStats::highscore = 0;

PlayerStats::PlayerStats(Player* _owner) {
	fruit = xlives = kills = deaths = powerups = 
	levels = warps = totaltime = hits = misses = fired = 0;

	owner = _owner;
	number = owner->playernumber + 1;
}

PlayerStats::~PlayerStats() {
	// delete[] out segfaults (?)
	//delete out;
}

void PlayerStats::hit() {
	hits++; //what about flamethrower
	fired++;
}

void PlayerStats::missed() {
	misses++;
	fired++;
}

void PlayerStats::died() {
	deaths++;
}

void PlayerStats::tookPowerup() {
	powerups++;
}

void PlayerStats::tookXlife() {
	xlives++;
}

void PlayerStats::tookFruit() {
	fruit++;
}

void PlayerStats::kill() {
	kills++;
}

void PlayerStats::wonLevel(int timeended, int timerequired) {
	levels++;
	totaltime += timeended;
	if (timeended < timerequired) {
		int warpfactor = ((timerequired - timeended)/5)+1;
		warps += warpfactor;
	}
}

int PlayerStats::calcScore() {
	//int levelon = 1 + levels + warps*(globals->loadInt("Constants/levelskip")-1);
	//int killrate = 100 * kills / (totaltime + 1);
	//int score = levelon/2 + killrate/50 + powerups/10;
	
	int score =	kills + 
			5*fruit +
			10*powerups +
			20*xlives +
			50*warps;

	//cout << kills << " kills, " << fruit << " fruit, " << powerups << " powerups, " << xlives << " extra lives, " << warps << " warped." << endl;
	enterScore(score);
	return score;
}

std::string PlayerStats::name() {
	char outc[1024];
	sprintf(outc, "Player %d:", number);
	return std::string(outc);
}

std::string PlayerStats::score() {
	char outc[1024];
	sprintf(outc, "%d Points", calcScore());
	return std::string(outc);
}

std::string PlayerStats::getHighScoreStr() {
	char outc[1024];
	sprintf(outc, "Highscore: %d Points", getHighScore());
	return std::string(outc);
}

std::string PlayerStats::rating() {
	std::string prize;
	int scored = calcScore();

	switch (scored / 50) {
		case 0: prize = "Shame"; break;
		case 1:
		case 2: prize = "A Participation Certificate"; break;
		case 3:
		case 4: prize = "A Free Councilling Session"; break;
		case 5: prize = "The Taine Randall Trophy for Losing"; break;
		case 6:
		case 7: prize = "Player Most Likely to Submit 'Easy-Mode' Patch"; break;
		case 8:
		case 9: prize = "The VA Linux $2 Sponsorship"; break;
		case 10:
		case 11: prize = "An extra life. Oops, too late."; break;
		case 12:
		case 13: prize = "The John F. Kennedy Memorial Cheeseboard"; break;
		case 14:
		case 15: prize = "Commendable Use of 'Use' key"; break;
		case 16:
		case 17: prize = "A painting by RICK MOEN"; break;
		case 18:
		case 19: prize = "A Quad Damage signed by John Carmack Himself"; break;
		case 20:
		case 21: prize = "A natty Rambo-esque headband"; break;
		case 22: prize = "Some Slack"; break;
		case 23: prize = "A Kitten. Robotfindskitten, I win!!"; break;
		case 24:
		case 25: prize = "The MS Windows prize for violent collisions"; break;
		case 26:
		case 27: prize = "Three servings of gravy"; break;
		case 28:
		case 29: prize = "Budget cuts"; break;
		case 30:
		case 31: prize = "RSI/OOS"; break;
		case 32:
		case 33: prize = "Miss NRA 2001"; break;
		case 34:
		case 35: prize = "Funding from the Badass Research Center"; break;
		case 36:
		case 37: prize = "Mass 0wn4ge"; break;
		case 38: prize = "The Rupert Murdoch prize for Taking Prisoners"; break;
		case 39:
		case 40: prize = "Gratitude from masses of released red pixels"; break;
		case 41:
		case 42: prize = "The meaning of life"; break;
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
		case 49:
		case 50: prize = "An overflow error"; break; // not really, it's a joke
		default: prize = "Something nondescript"; break;
	}
	
	char outc[1024];
	sprintf(outc, "You have been awarded:\n%s", prize.c_str());
	return std::string(outc);
}

/*
std::string PlayerStats::report() {
	int accuracy = 100 * hits / (fired+1);
	int levelon = 1 + levels + warps*(globals->loadInt("Constants/levelskip")-1);
	int avtime = totaltime / (levels+1);
	int killrate = 100 * kills / (totaltime + 1);

	char outc[1024];
	sprintf(outc, "%s\n\n\n
		You died on level %d, after losing %d lives,\n 
		having completed %d levels and warping %d times.\n\n
		It took an average of %d seconds to clear a level.\n
		You've been shooting with %d percent accuracy.\n
		You've killed at %d.%d monsters per second.\n
		Total game time: %d minutes, %d seconds.\n", 
		(rating()).c_str(), levelon, deaths, levels, warps, avtime, 
		accuracy, killrate/100, killrate%100, totaltime/60, totaltime%60);
	return std::string(outc);
}
*/
