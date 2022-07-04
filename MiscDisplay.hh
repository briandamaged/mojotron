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

#ifndef MISCDISPLAY_HH
#define MISCDISPLAY_HH

#include "SDL_CL_Surface.h"

class Sprite;
class PlayerStats;
	
class MiscDisplay {
public:
	MiscDisplay();
	~MiscDisplay();
	void setConsole(std::string newstr, int duration);
	void drawConsole();
	void drawMisc();
	void drawLevWarp(int xorigin, int yorigin, int time);
	void drawHelpBonus(int time);
	void drawWonGame();
	void drawClock(int xorigin, int yorigin);
	void drawLevel(int xorigin, int yorigin);
	void drawLives(int xorigin, int yorigin);

private: 
	int consoletimer;
	std::string consoledisplay;

	static CL_Surface* livesicon;
	static CL_Surface* levelicon;
	static CL_Surface* timeicon;
};

#endif
