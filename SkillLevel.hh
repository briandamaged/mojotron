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

#ifndef SKILLLEVEL_HH
#define SKILLLEVEL_HH

#include <string>

#define MAX_SKILL_LEVS 3

class SkillLevel {
public:
	int level;
	int speed;
	int quantity;
	std::string name;

	static void initAll();
	static void deinitAll();
	static void cycleSkill(bool higher);
	static void setSkill(int level);
	static SkillLevel* current;
	static SkillLevel* skills[MAX_SKILL_LEVS];

private:
	static int difficulty;

	SkillLevel(int level);
	~SkillLevel();
};

#endif

