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

#ifndef INTRO_HH
#define INTRO_HH

#include "SDL_CL_Surface.h"
#include <string>

#define XKEYCENT 20
#define YKEYCENT 12
#define KEYWIDTH 48
#define KEYHEIGHT 48
#define STORYTIME storytime
#define CONTROLSSTART (STORYTIME+9000)
#define POWERUPSTART (CONTROLSSTART+9000)
#define FRUITSTART (POWERUPSTART+9000)
#define ENEMYSTART (FRUITSTART+9000)
#define RESTART (ENEMYSTART+0)

class Demo;

class IntroThing {
public:
	IntroThing(int x1, int y1, int f, CL_Surface a);
	virtual ~IntroThing();
	virtual void act(int timer) = 0;
	virtual void draw();

protected:
	int x, y;
	int frame;
	CL_Surface anim;
};

class IntroPlayer : public IntroThing {
public:
	IntroPlayer(int x1, int y1, int p);
	void act(int timer);
	void draw();

	int player;
	CL_Surface armanim;
};

class IntroHuman : public IntroThing {
public:
	IntroHuman(int x1, int y1, int t);
	void act(int timer);

	int type;

	static const std::string typesurface[3];
};

class IntroBubble : public IntroThing {
public:
	IntroBubble(int x1, int y1);
	void act(int timer);
};

class IntroRobot : public IntroThing {
public:
	IntroRobot(int x1, int y1, int t);
	void act(int timer);

	int type;

	static const std::string typesurface[3];
};

class IntroSpecification {
public:
	IntroSpecification();

	void load(std::string prefix);
	IntroThing* instantiate();

	int type;
	int x, y;
	int data;
};

class IntroScene {
public:
	std::vector<std::string> text;
	std::vector<int> texty;
	std::vector<IntroSpecification> specs;
	int storytime;
};

class Intro {
public:
	Intro();
	~Intro();

	bool show();
	static void drawBg();
	void demoBg();
	void introBg();

	static CL_Surface* sur_skillicons;
	static Demo* demo;
private:
	void story();
	void powerups();
	void fruitTable();
	void enemies();
	void drawEnemyLine(	int line, int s,
					std::string name, std::string info);
	void controlsDemo();
	void restart(bool active);

	static int timer;
	static int delta;
	static int prevtime;
	static int demotime;
	static int restarttime;
	static int introstarttime;
	static int slomoval;

	static int storytime;

	static std::string subtitle;

	CL_Surface* sur_bubble;
	static CL_Surface* sur_starfield;
	CL_Surface* sur_title;
	CL_Surface* sur_reel;
	CL_Surface sur_view;
	CL_Surface sur_viewport;
	static int fruit[9];
	static int textstart;
	static int textincrement;
	int currentscene;
	std::vector<IntroScene> scenes;
	std::vector<std::unique_ptr<IntroThing>> things;
};

#endif
