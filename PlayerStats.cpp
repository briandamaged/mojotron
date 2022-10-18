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
#include "Application.h"
#include "PlayerStats.hh"
#include "Player.hh"

#ifdef HAVE_GAMERZILLA
#include <gamerzilla.h>
#endif

using namespace std;
extern Globals* globals;

int PlayerStats::highscore = 0;

PlayerStats::PlayerStats(Player* _owner) {
	owner = _owner;
	number = owner->playernumber + 1;
}

PlayerStats::~PlayerStats() {
}

void PlayerStats::hit() {
	current.hits++; //what about flamethrower
	current.fired++;
	if (number == 1) {
		Config::config.overall.hits++;
		Config::config.overall.fired++;
		if (Config::config.best.hits < current.hits)
			Config::config.best.hits = current.hits;
		if (Config::config.best.fired < current.fired)
			Config::config.best.fired = current.fired;
	}
}

void PlayerStats::missed() {
	current.misses++;
	current.fired++;
	if (number == 1) {
		Config::config.overall.misses++;
		Config::config.overall.fired++;
		if (Config::config.best.misses < current.misses)
			Config::config.best.misses = current.misses;
		if (Config::config.best.fired < current.fired)
			Config::config.best.fired = current.fired;
	}
}

void PlayerStats::died() {
	current.deaths++;
	if (number == 1) {
		Config::config.overall.deaths++;
		if (Config::config.best.deaths < current.deaths)
			Config::config.best.deaths = current.deaths;
	}
}

void PlayerStats::tookPowerup() {
	current.powerups++;
	if (number == 1) {
		Config::config.overall.powerups++;
		if (Config::config.best.powerups < current.powerups)
			Config::config.best.powerups = current.powerups;
	}
}

void PlayerStats::tookXlife() {
	current.xlives++;
	if (number == 1) {
		Config::config.overall.xlives++;
		if (Config::config.best.xlives < current.xlives)
			Config::config.best.xlives = current.xlives;
	}
}

void PlayerStats::tookFruit() {
	current.fruit++;
	if (number == 1) {
		Config::config.overall.fruit++;
		if (Config::config.best.fruit < current.fruit)
			Config::config.best.fruit = current.fruit;
	}
}

void PlayerStats::kill() {
	current.kills++;
	if (number == 1) {
		Config::config.overall.kills++;
		if (Config::config.best.kills < current.kills)
			Config::config.best.kills = current.kills;
	}
}

void PlayerStats::wonLevel(int timeended, int timerequired) {
	current.levels++;
	current.totaltime += timeended;
	if (number == 1) {
		Config::config.overall.levels++;
		Config::config.overall.totaltime += timeended;
		if (Config::config.best.levels < current.levels)
			Config::config.best.levels = current.levels;
		if (Config::config.best.totaltime < current.totaltime)
			Config::config.best.totaltime = current.totaltime;
	}
	if (timeended < timerequired) {
		int warpfactor = ((timerequired - timeended)/5)+1;
		current.warps += warpfactor;
		if (number == 1) {
			Config::config.overall.warps += warpfactor;
			if (Config::config.best.warps < current.warps) {
#ifdef HAVE_GAMERZILLA
				GamerzillaSetTrophyStat(Application::getApplication()->game_id, "First Warp", 1);
#endif
				Config::config.best.warps = current.warps;
			}
		}
	}
}

void PlayerStats::lostLevel(int timeended) {
	current.totaltime += timeended;
	if (number == 1) {
		Config::config.overall.totaltime++;
		if (Config::config.best.totaltime < current.totaltime)
			Config::config.best.totaltime = current.totaltime;
	}
}

int PlayerStats::calcScore() {
	//int levelon = 1 + levels + warps*(globals->loadInt("Constants/levelskip")-1);
	//int killrate = 100 * kills / (totaltime + 1);
	//int score = levelon/2 + killrate/50 + powerups/10;
	
	int score =	current.kills +
			5*current.fruit +
			10*current.powerups +
			20*current.xlives +
			50*current.warps;

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
	int scored = calcScore();
	std::string prize = globals->ratings->rating(scored);
	
	char outc[1024];
	sprintf(outc, "You have been awarded:\n%s", prize.c_str());
	return std::string(outc);
}
