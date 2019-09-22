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

#include <ClanLib/core.h>
#include "SkillLevel.hh"
#include "Globals.hh"

using namespace std;
extern Globals* globals;

int SkillLevel::difficulty = 0;
SkillLevel* SkillLevel::current = NULL;
SkillLevel* SkillLevel::skills[];

void SkillLevel::initAll() {
	int ii;
	for (ii=0; ii < MAX_SKILL_LEVS; ii++) {
		skills[ii] = new SkillLevel(ii);
	}
	current = skills[difficulty];
}

void SkillLevel::deinitAll() {
	int ii;
	for(ii=0; ii < MAX_SKILL_LEVS; ii++) {
		delete skills[ii];
		skills[ii] = 0;
	}
}

void SkillLevel::cycleSkill(bool higher) {
	if (higher)	setSkill(difficulty+1);
	else		setSkill(difficulty-1);
}

void SkillLevel::setSkill(int level) {
	if (level < 0)
		level = MAX_SKILL_LEVS - 1;
	level %= MAX_SKILL_LEVS;
	difficulty = level;

	current = skills[difficulty];
}

SkillLevel::SkillLevel(int ii) {
	char ch[13];
	sprintf(ch, "SkillLevel%d/", ii);
	std::string base = string(ch);

	level = ii;
	name = globals->loadString(base + "name");
	speed = globals->loadInt(base + "speed");
	quantity = globals->loadInt(base + "quantity");
}

SkillLevel::~SkillLevel() {

}
