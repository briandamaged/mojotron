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

#ifndef PLAYERSTATS_HH
#define PLAYERSTATS_HH

#include <string>

class Player;

class PlayerStats {
public:
	PlayerStats(Player* _owner);
	~PlayerStats();

	void hit();
	void missed();
	void died();
	void kill();
	void tookPowerup();
	void tookXlife();
	void tookFruit();
	void wonLevel(int timeended, int timerequired);

	int calcScore();
	static void enterScore(int score)
		{ if (score > highscore)  highscore = score; }
	static int getHighScore() { return highscore; }
	static std::string getHighScoreStr();
	std::string name();
	std::string score();
	std::string report();
	std::string rating();
	int totaltime;

private:
	int hits, misses, fired, kills;
	int deaths, powerups, xlives, fruit;
	int levels, warps;
	int number;
	Player* owner;
	//std::string out;
	
	static int highscore;
};

#endif
