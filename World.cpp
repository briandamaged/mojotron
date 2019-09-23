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
#include <cstdio>
#include "World.hh"
#include "Thing.hh"
#include "Player.hh"
#include "Sprite.hh"
#include "Sound.hh"
#include "Hazards.hh"
#include "Barricade.hh"
#include "SkillLevel.hh"
#include "Bonus.hh"
#include "ScoreBoard.hh"
#include "MiscDisplay.hh"
#include "ObjectList.hh"
#include "Demo.hh"
#include "Menu.hh"

using namespace std;
World* worldobj;
extern Globals* globals;

World::World() {
	// General variables, global speed etc.
	orig_monster_speed = (float)globals->loadInt("Constants/monsterspeed")/100;
	monster_speed = orig_monster_speed;
	monster_size = 0;	// zero is no adjustment

	num_players = globals->num_players;
	monster_quantity = (float)SkillLevel::current->quantity/100;
	monster_quantity *= (1.0f + (float)(num_players - 1) * 0.5f);

	lives = num_players*globals->loadInt("PlayerSpecs/startlives");
	all = new ObjectList(MAX_OBJS);
	Bonus::initProb();

	// build the player list
	for (int i=0; i < num_players; i++) {
		makePlayer(i);
	}
	elist = new ObjectList(MAX_OBJS);
	ellist = new ObjectList(MAX_OBJS);
	pllist = new ObjectList(MAX_OBJS);
	olist = new ObjectList(MAX_OBJS);
	ollist = new ObjectList(MAX_OBJS);
	
	miscdisplay = new MiscDisplay();
	pausemenu = new Menu(Menu::INGAME);

	spr_grunt = 0;
	base = 0;
	flashing = 0;	statetime = 0;
}

World::~World() {
	/* Deletes every screen object, even the players */
	Thing* temp;
	while ((temp = all->getNext())) {
		if (globals->verbosity > 1)
			cout << "Deleting Thing: " << temp << endl;
		delete temp;
	}
	/* Delete the empty lists */
	delete all;
	delete elist;
	delete ellist;
	delete pllist;
	delete olist;
	delete ollist;

	delete base;

	if (!globals->loadInt("Testing/singlegruntsprite")) {
		// will be null if world deleted immediately
		// I guess deleting null pointers is harmless
		delete spr_grunt;
	}
	delete miscdisplay;
	delete pausemenu;
	worldobj = NULL;
}

/*
 * LEVEL LOADING
 ****************/

void World::load_level(int l) {
	try {
		int loadstarttime = CL_System::get_time();
		globals->loadingScreen(loadstarttime);

		num_lev = l;

		// make char array string containing level num. extension
		char number[3];
		sprintf(number, "%d", l);
		
		// put the directory for the current level in string 'base'
		if (base)	delete base;
		base = new std::string("Levels/level/");
		base->insert((unsigned int)12, number);
		if (globals->verbosity > 0)
			cout << "Loading: " << *base << endl;
		
		lev_name = loadlevString("lev_name");
		warp_time = loadlevInt("warp_time");
		warp_time += (int)((monster_quantity - 1.0f) * warp_time / 2);
		warp_time /= num_players;

		// increase the player's stats
		for (int i=0; i < num_players; i++) {
			playerlist[i]->speedUp(num_lev / 10);
		}

		bgred = loadlevInt("bg_red");
		bggreen = loadlevInt("bg_green");
		bgblue = loadlevInt("bg_blue");
		
		globals->loadingScreen(loadstarttime);

		// Looking for initial numbers of monsters etc.
		// Oh no! This method needs to know about every kind of monster/hazard in the game.
		int num_balls = loadlevInt("num_balls");
		int num_obst = loadlevInt("num_obst");
		int num_grunts = loadlevInt("num_grunts");
		int num_spikers = loadlevInt("num_spikers");
		int num_asteroids = loadlevInt("num_asteroids");
		int num_snipers = loadlevInt("num_snipers");
		int num_rightanglers = loadlevInt("num_rightanglers");
		int num_simpleboss = loadlevInt("num_simpleboss");
		int num_rockmen = loadlevInt("num_rockmen");
		int num_turrets = loadlevInt("num_turrets");
		int num_soldiers = loadlevInt("num_soldiers");
		int num_circleboss = loadlevInt("num_circleboss");
		int num_centipedes = loadlevInt("num_centipedes");
		int num_cellphones = loadlevInt("num_cellphones");
		int num_spirals = loadlevInt("num_spirals");
		//int num_conglomerates = loadlevInt("num_conglomerates");
		int num_lasers = loadlevInt("num_lasers");
		int num_barricades = loadlevInt("num_barricades");
		int num_fruit = loadlevInt("num_fruit");
		
		// used in empty.scr to make sure no SPs are loaded twice
		if (globals->loadInt("Testing/singlegruntsprite")) {
			spr_grunt = globals->spr[Globals::SINGLEGRUNT];
		} else {
			if (spr_grunt) {
				delete spr_grunt;
			}
			std::string temp = std::string("Surfaces/");
			temp.append(loadlevString("Grunt"));
			spr_grunt = new Sprite(temp, globals->manager);
		}
		globals->loadingScreen(loadstarttime);

		int gruntspeed = loadlevInt("gruntspeed");
		int grunthealth = loadlevInt("grunthealth");
		bool gruntescapepod = loadlevInt("gruntescapepod");
		bool gruntrandompod = loadlevInt("gruntrandompod");
		bool gruntbouncingpod = loadlevInt("gruntbouncingpod");
		int rightanglermix = loadlevInt("rightanglermix"); // 0 for random, 1 for all on walls, 2 for all on roof/floor
		int turretfansize = loadlevInt("turretfansize");
		int barricadeson = loadlevInt("dropbarricades"); // 1 dead enemies drop barricades, 0 normal
		int centipedelength = loadlevInt("centipedelength");
	
		bool fruittypes[6];
		fruittypes[0] = (bool)loadlevInt("specialfruit");
		fruittypes[1] = (bool)loadlevInt("redfruit");
		fruittypes[2] = (bool)loadlevInt("orangefruit");
		fruittypes[3] = (bool)loadlevInt("yellowfruit");
		fruittypes[4] = (bool)loadlevInt("greenfruit");
		fruittypes[5] = (bool)loadlevInt("bluefruit");
		
		// Construct Things
		all->clearLevel();
		int n;

		globals->loadingScreen(loadstarttime);

		// monsters required killed
		for(n=0; n < (int)(monster_quantity * num_balls); n++) {
			if (globals->verbosity > 1) cout << "z";
			all->addThing(new Ball(Globals::BALL));
		}

		globals->loadingScreen(loadstarttime);

		for(n=0; n < (int)(monster_quantity * num_grunts); n++) {
			if (globals->verbosity > 1) cout << "y";
			Thing* temp = new Grunt(Globals::SINGLEGRUNT, gruntspeed);
			temp->spr = spr_grunt;
			temp->setupAnim();

			if (grunthealth) temp->health = grunthealth;
			if (barricadeson) temp->dropsbarricade = true;
			all->addThing(temp);
		}
		Grunt::escapepod = gruntescapepod;
		Grunt::randompod = gruntrandompod;
		Grunt::bouncingpod = gruntbouncingpod;

		globals->loadingScreen(loadstarttime);

		for(n=0; n < (int)(monster_quantity * num_spikers); n++) {
			if (globals->verbosity > 1) cout << "s";
			all->addThing(new Spiker(Globals::SPIKER));
		}
		for(n=0; n < (int)(monster_quantity * num_asteroids); n++) {
			if (globals->verbosity > 1) cout << "a";
			all->addThing(new Asteroid());
		}
		for(n=0; n < (int)(monster_quantity * num_snipers); n++) {
			if (globals->verbosity > 1) cout << "g";
			all->addThing(new Sniper(Globals::SNIPER));
		}
		for(n=0; n < (int)(monster_quantity * num_rightanglers); n++) {
			if (globals->verbosity > 1) cout << "g";
			if (rightanglermix == 1) {
				all->addThing(new RightAngler(Globals::RIGHTANGLER, true));
			} else if (rightanglermix == 2) {
				all->addThing(new RightAngler(Globals::RIGHTANGLER, false));
			} else {
				all->addThing(new RightAngler(Globals::RIGHTANGLER, (rand() < RAND_MAX/2)));
			}
		}
		globals->loadingScreen(loadstarttime);

		for(n=0; n < (int)(monster_quantity * num_turrets); n++) {
			all->addThing(new Turret(Globals::TURRET, turretfansize));
		}
		for(n=0; n < (int)(monster_quantity * num_soldiers); n++) {
			Thing* temp = new Soldier(Globals::SOLDIER);
			if (barricadeson) temp->dropsbarricade = true;
			all->addThing(temp);
		}
		globals->loadingScreen(loadstarttime);
		for(n=0; n < (int)(monster_quantity * num_simpleboss); n++) {
			all->addThing(new SimpleBoss(Globals::SIMPLEBOSS, n));
		}
		for(n=0; n < (int)(monster_quantity * num_circleboss); n++) {
			// evenly distribute the bosses around their circles, and make one circle 180deg to the other
			all->addThing(new CircleBoss(Globals::CIRCLEBOSS, 
							ARENAWIDTH/3 * (1 + n%2), ARENAHEIGHT/2, gruntspeed, 
							(n/2)*4*M_PI/num_circleboss + (1 + n%2)*M_PI)); 
		}
		globals->loadingScreen(loadstarttime);

		for(n=0; n < (int)(monster_quantity * num_centipedes); n++) {
			Thing* head = new Centipede(Globals::CENTIHEAD, 0);
			all->addThing(head);

			Thing* leadby = head;
			for(int j=1; j < centipedelength; j++) {
				Thing* temp = new Centipede(Globals::CENTISEG, leadby);
				all->addThing(temp);
				leadby = temp;
			}
			head->startPos();	// special case. Other monsters should call their own startPos
		}

		globals->loadingScreen(loadstarttime);

		Thing* previous = 0;
		for(n=0; n < (int)(monster_quantity * num_cellphones); n++) {
			Thing* temp = new Cellphone(Globals::CELLPHONE, previous);
			all->addThing(temp);
			previous = temp;
		}
		for(n=0; n < (int)(monster_quantity * num_spirals); n++) {
			all->addThing(new Spiral(Globals::SPIRAL));
		}
		max_active = num_active = all->getNumToBeKilled();

		globals->loadingScreen(loadstarttime);

		// optionally killed
		for(n=0; n < (int)(monster_quantity * num_obst); n++) {
			if (globals->verbosity > 1) cout << "x";
			all->addThing(new Obstacle(Globals::OBSTACLE));
		}
		for(n=0; n < num_fruit; n++) {
			if (globals->verbosity > 1) cout << "f";
			all->addThing(new Fruit(fruittypes));
		}

		globals->loadingScreen(loadstarttime);

		for(n=0; n < (int)(monster_quantity * num_rockmen); n++) {
			if (globals->verbosity > 1) cout << "r";
			all->addThing(new RockMan(Globals::ROCKMAN));
		}
		for(n=0; n < num_barricades; n++) {
			all->addThing(new Barricade());
		}
		for(n=0; n < (int)(monster_quantity * num_lasers); n++) {
			if (rand() > RAND_MAX/2)
				all->addThing(new Laser(Globals::LASERVERT, true));
			else
				all->addThing(new Laser(Globals::LASERHORIZ, false));
		}
		globals->loadingScreen(loadstarttime);
		
		for(int i=0; i < num_players; i++) {
			playerlist[i]->startPos();
		}
		globals->loadingScreen(loadstarttime);

	} catch(CL_Error err) {
		cout << "CL_Error: " << err.message.c_str() << std::endl;
	}
}

// templates would probably be the good thing here

int World::loadlevInt(std::string dataname) {
	// loads /Levels/levelxx/dataname, from the resource file
	std::string temp = *base;
	temp.append(dataname);
	return (globals->loadInt(temp));
}

std::string World::loadlevString(std::string dataname) {
	// loads /Levels/levelxx/dataname, from the resource file
	std::string temp = *base;
	temp.append(dataname);
	return (globals->loadString(temp));
}

/*
 * DRAWING
 **********/

std::string World::serialiseBG() {
	char buffer[14];
	sprintf(buffer, "BG:%3d%3d%3d ",
		flashing + bgred,
		flashing + bggreen,
		flashing + bgblue);
	return std::string(buffer);
}

void World::drawBG(int delta) {
	if (flashing) {
		flashing -= (delta/4);
		if (flashing < 0) flashing = 0;
	}
	CL_Display::fill_rect(	0, 0,
				(ARENAWIDTH>>8), (ARENAHEIGHT>>8)+PAD,
				getBGRed(), getBGGreen(), getBGBlue(), 1.0f);
}

void World::drawScoreBG() {
		CL_Display::fill_rect(	0, (ARENAHEIGHT>>8) + PAD,
					(XWINSIZE>>8), (YWINSIZE>>8),
					worldobj->getBGRed()/1.3f, 
					worldobj->getBGGreen()/1.3f, 
					worldobj->getBGBlue()/1.3f, 1.0f);
}

float World::getBGRed() {
	float r = (float)(flashing + bgred)/100;
	if (r > 1.0f)	r = 1.0f;
	return r;
}

float World::getBGGreen() {
	float g = (float)(flashing + bggreen)/100;
	if (g > 1.0f)	g = 1.0f;
	return g;
}

float World::getBGBlue() {
	float b = (float)(flashing + bgblue)/100;
	if (b > 1.0f)	b = 1.0f;
	return b;
}

void World::updateDisplay() {
	// don't draw playfield if we're faded out
	if (!(	(statetime > 3000 && state == GAMEOVER) ||
		pausemenu->type == Menu::OPTIONS)) // or are in the option menu
	{	
		// draw playfield
		drawBG(delta);
		all->toListStart();	Thing* x;
		while ((x = all->getNext())) {
			x->draw();
		}
	}

	if (state != GAMEOVER) {
		drawScoreBG();
		for (int i=0; i < num_players; i++) {
			playerlist[i]->scoreboard->drawScoreBoard(
				(ARENAWIDTH>>8) - (i+1)*BOARDWIDTH,
				(ARENAHEIGHT>>8) + PAD);
		}
		miscdisplay->drawMisc();
		miscdisplay->drawConsole();

		switch (state) {
			case WARPING: {
				int factor = (getWarpRemaining() / 5) + 1;
				miscdisplay->drawLevWarp(1,
					factor*globals->loadInt("Constants/levelskip"), statetime);
				break;
			}
			case HELPING: {
				miscdisplay->drawHelpBonus(statetime);
				break;
			}
			case ENDGAME: {
				miscdisplay->drawWonGame();
				break;
			}
			case PAUSED: {
				globals->largefont->print_center(XWINSIZE>>9, YWINSIZE>>9, "Paused");

				int pausedelta = CL_System::get_time() - pausetimer;
				pausetimer = CL_System::get_time();

				Menu::menustatus ret = pausemenu->run(pausedelta);
				if (ret == Menu::STARTGAME) {
					state = PLAYING;
					// resync time
					cltime = CL_System::get_time();
					readyforkey = false;
					pausemenu->readyforkey = false;
				} else if (ret == Menu::EXIT) {
					state = EXIT;
				}
				break;
			}
			case READY: {
				globals->largefont->print_center(XWINSIZE>>9, YWINSIZE>>9, "Ready...");
				break;
			}
			case PLAYING:
			case DIED:
			case WON:
			case NEXTLEV:
			case WARPED:
			case GAMEOVER:
			case EXIT:
			case DELAY:
				;
		}

	} else {
		if (statetime > 3000) {	// scrolling
			CL_Display::clear_display(0, 0, 0, 1.0f);
			globals->smallfont->print_right((XWINSIZE>>8) - 10, 10, 
				PlayerStats::getHighScoreStr());

			for (int i=0; i < num_players; i++) {
				float alpha = ( (float)(statetime - 3000) / 1000.0f
						- (float)((i * 2) + 1));
				if (alpha < 0.0f)	alpha = 0.0f;
				if (alpha > 1.0f)	alpha = 1.0f;

				int textbegin = 180;
				int textheight = 100;
				globals->smallfont->print_left(	100, textbegin + textheight*i,
								(playerlist[i]->stats->name()).c_str());
				globals->smallfont->print_right((XWINSIZE>>8) - 100, textbegin + textheight*i,
								(playerlist[i]->stats->score()).c_str());

				globals->spr[i]->draw( 100<<8, (textbegin + textheight*i + textheight/3)<<8, 0, false );

				globals->smallfont->print_left(160, textbegin + textheight*i + textheight/3,
								(playerlist[i]->stats->rating()).c_str());

				if (alpha < 0.99f) {
					CL_Display::fill_rect(	0, textbegin + textheight*i,
								(XWINSIZE>>8), textbegin + textheight*(i+1),
								0.0, 0.0, 0.0, 1-alpha);
				}
				if (statetime > 7000) {
					globals->mediumfont->print_center((XWINSIZE>>9), (YWINSIZE>>8) - 25, 
									"Press any key to continue");
				}
			}
		} else {
			float alpha = (float)(statetime)/3000;
			CL_Display::clear_display(0, 0, 0, alpha);
		}
		if (lev_name == "")
			globals->largefont->print_center(XWINSIZE>>9, 110, "Congratulations!");
		else
			globals->largefont->print_center(XWINSIZE>>9, 110, "GAME OVER");
				
	}

	CL_Display::flip_display();
}

/*
 * GAME LOOP
 ************/

// sends back the number of the next level, or 0 if quit or ran out of lives
int World::run(bool recorddemo) {
	Demo demo = Demo();
	globals->unfade();

	cltime = CL_System::get_time();	// don't get the time directly, use this
	oldtime = CL_System::get_time();
	inittime = oldtime;
	int maxdelta = globals->loadInt("Constants/maxdelta");
	int mindelta = globals->loadInt("Constants/mindelta");
	int startaccellength = globals->loadInt("Constants/startaccelerationlength");
	startaccel = 0.0f;
	
	levelage = timesinceready = 0;
	unsigned long framenum = 0;
	
	delay = 0;
	
	if 	(lev_name == "") state = ENDGAME;
	else 	gameDelay(	globals->loadInt("Constants/readypause"),
				READY, PLAYING );
	
	char levchars[20];
	sprintf(levchars, "Level %d: ", num_lev);
	std::string levstring = std::string(levchars);
	miscdisplay->setConsole(levstring.append(lev_name), globals->loadInt("Constants/levelinfotimeout"));
	Sound::playSound("beginlevel");

	int framedone = CL_System::get_time();
	while 	(true) 
	{
		delta = 0;

		switch(state) {
			case PLAYING: {
				// don't want time updated while paused
				cltime = CL_System::get_time();
				delta = cltime - oldtime;

				// time since ready is in real millisecs, not game millisecs
				timesinceready += delta;
				startaccel = 0.5f;
				startaccel += (float)timesinceready / (startaccellength+1);
				if (startaccel > 1.0f)	startaccel = 1.0f;
				// a percentage
				deltamodify = (int)(SkillLevel::current->speed * startaccel);

				delta = delta*deltamodify / 100;

				// if we get an excessivly large delta, don't act on it.
				if (delta > maxdelta)	delta = 0;
				
				listenPauseToggle();	
				
				if (!num_players) state = GAMEOVER;
				if (!num_active) state = WON;
				
				Grunt::calcSpeed(delta, max_active-num_active);
				
				// move all
				all->toListStart();	Thing* x;
				while ((x = all->getNext())) {
					x->move(delta);
				}
				all->clearDeadStuff();

				if (recorddemo)	demo.writeSnapshot(timesinceready);

				// Check for collisions, and call event methods
				makeLists();
				findEnemyCols();
				findPlayerCols();
				findObstacleCols();

				break;
			}
		
			case WON: {
				Sound::playSound("levelover");
				for(int i=0; i < num_players; i++) {
					playerlist[i]->stats->wonLevel(getLevelAge()/1000, warp_time);
				}
				if (warp_time > (getLevelAge()/1000)) {
					gameDelay(globals->loadInt("Constants/warppause"), 
						World::WARPING, World::WARPED);
					lives++;
				} else if (lives == 0) {
					gameDelay(globals->loadInt("Constants/warppause"),
						World::HELPING, World::NEXTLEV);
					lives++;
				} else {
					//return (num_lev + 1);
					state = NEXTLEV;
				}
				break;
			}

			case WARPED: {
				int factor = (getWarpRemaining() / 5) + 1;
				return (num_lev + factor*globals->loadInt("Constants/levelskip"));
			}

			case NEXTLEV: {
				return (num_lev + 1);
			}

			case EXIT: {
				//if (globals->verbosity > 0)
					cout << endl << endl << "Average rate: " << 
						1000 * framenum / (CL_System::get_time() - inittime) << "fps" << endl;
				return 0;
			}

			case GAMEOVER:
			case ENDGAME: {
				delay = 0;	// goes on until user presses key

				// listen for any key after the first few moments
				if (listenAnyKey() && statetime > 5000) {
					if (state == GAMEOVER)	return 0;	// exit

					// user's finished looking at win message
					gameDelay(100, World::DELAY, World::GAMEOVER);
				}
				break;
			}

			case DIED: {
				restart();
				all->clearDeadStuff();
				break;
			}

			case PAUSED: {
				listenPauseToggle();
				break;
			}

			case READY:
			case WARPING:
			case HELPING:
			case DELAY: {
			}
		}
		maintainDelay();

		// crude profiling stuff
		// int gamelogictime = CL_System::get_time() - cltime;
		
		updateDisplay();
		
		//int graphicstime = (CL_System::get_time() - cltime) - gamelogictime;
		
		CL_System::keep_alive();

		int frametime = CL_System::get_time() - framedone;
		if (frametime < mindelta)
			CL_System::sleep(mindelta - frametime);
		framedone = CL_System::get_time();

		levelage += delta;
		oldtime = cltime;
		framenum++;

		if (globals->verbosity > 1) {
			cout << "num_active = " << num_active << "\tdelta = " << delta << endl;
			//cout << "Logic took: " << gamelogictime << "\tGraphics took: " << graphicstime << endl;
		}
	}
	return 0;	// never reached, see state = EXIT
}

/*
 * GAME STATE CONTROL
 *********************/

void World::gameDelay(int duration, World::GameState stateduring, World::GameState stateafter) {
	oldstate = stateafter; // temporary storage
	state = stateduring;
	delay = (unsigned long)duration;
	flashing = 0;
}

void World::maintainDelay() {
	statetime = (int)(CL_System::get_time() - cltime);

	if (delay) {
		if (statetime > delay) {
			state = oldstate;
			cltime = CL_System::get_time();
			statetime = 0;
		}
	} // if delay is zero, this state goes on forever
}

void World::restart() {
	oldtime = CL_System::get_time(); // avoid a large delta being made
	
	all->toListStart();	Thing* x;
	while ((x = all->getNext())) {
		x->startPos();
	}
	
	// slow down the grunts a bit, give them a chance
	Grunt::agespeedup -= globals->loadInt("Constants/playerdeathslowdown");
	if (Grunt::agespeedup < 0) Grunt::agespeedup = 0;

	if (globals->verbosity > 0) {
		cout << "Grunt speed status:" << endl;
		cout << "Age increase = " << Grunt::agespeedup << endl << "Low numbers increase = " << Grunt::numspeedup << endl;
	}

	timesinceready = 0;
	statetime = 0;
	gameDelay( globals->loadInt("Constants/readypause"), READY, PLAYING);
}

/*
 * ACCESSORS
 ************/

int World::getLevelAge() {
	return levelage;
}

Thing* World::getNearestPlayer(int xpos, int ypos) {
	if (num_players == 1)
		return playerlist[0];
		
	int challenger, defender;
	Thing* bestcandidate;

	int xdist = playerlist[0]->xpos-xpos;
	int ydist = playerlist[0]->ypos-ypos;
	defender = xdist*xdist + ydist*ydist;
	bestcandidate = playerlist[0];

	for (int i=1; i < num_players; i++) {
		xdist = playerlist[i]->xpos-xpos;
		ydist = playerlist[i]->ypos-ypos;
		challenger = xdist*xdist + ydist*ydist;

		if (challenger < defender) {
			defender = challenger;
			bestcandidate = playerlist[i];
		}
	}
	return bestcandidate;
}

void World::addThing(Thing* newthing) {
	if (!(newthing->optionalkill))
		num_active++;
	all->addThing(newthing);
}

// N.B  Does not delete object, just takes it off the list
void World::removeThing(Thing* deadthing) {
	if (!(deadthing->optionalkill))
		num_active--;
	all->unlistThing(deadthing);
}

void World::serialiseAll() {
	Thing* t;
	all->toListStart();
	while ((t = all->getNext())) {
		printf("%d %d\t%d %d %d\n",
			(t->xpos>>8), ((t->ypos - t->zpos)>>8), 
			t->sprnumber, t->frame, t->facingleft);
	}
	printf("\n");
}

/*
 * EVENT SYSTEM
 ***************/

// CHANGES
void World::makeLists() {
	elist->clearData();
	ellist->clearData();
	pllist->clearData();
	olist->clearData();
	ollist->clearData();

	all->toListStart();	Thing* temp;
	while ((temp = all->getNext())) {
		if (temp->getType() == Thing::ENEMY)	elist->addThing(temp);
		if (temp->getType() == Thing::OBSTACLE) olist->addThing(temp);
		if (temp->listensPlayer())		pllist->addThing(temp);
		if (temp->listensEnemy())		ellist->addThing(temp);
		if (temp->listensObstacle())		ollist->addThing(temp);
	}
}

// CHANGES
void World::findEnemyCols() {
	Thing* currentus, *currentversus;

	while ((currentus = elist->getNext())) {
		while ((currentversus = ellist->getNext())) {
			// check for things listening to themselves
			if (currentversus != currentus) {

				if (currentversus->colcheck(currentus)) {
					currentversus->tookEnemy(currentus);
				}
			} 
		}
		ellist->toListStart();
	}
}

void World::findObstacleCols() {
	Thing* currentus, *currentversus;

	while ((currentus = olist->getNext())) {
		while ((currentversus = ollist->getNext())) {
			// check for things listening to themselves
			if (currentversus != currentus) {

				if (currentversus->colcheck(currentus)) {
					currentversus->tookObstacle(currentus);
				}
			} 
		}
		ollist->toListStart();
	}
}

// CHANGES
void World::findPlayerCols() {
	Thing* currentversus;

	while ((currentversus = pllist->getNext())) {
		for(pindex=0; pindex < num_players; pindex++) {
			// check for things listening to themselves
			if (currentversus != playerlist[pindex]) {

				if (currentversus->colcheck(playerlist[pindex])) {
					currentversus->tookPlayer(playerlist[pindex]);
				}
			} 
		}
		//versus->toListStart();
	}
}

// add a findObstacleCols

/*
 * LIST HANDLING
 ****************/

void World::makePlayer(int i) {
	Player* newplay = new Player(i);
	playerlist[i] = newplay;
	all->addThing(newplay);
	newplay->playing = true;
}


/*
 * MISC INPUT STUFF
 *******************/

void World::listenPauseToggle() {
	bool pause = 	CL_Keyboard::get_keycode(CL_KEY_P) || 
			CL_Keyboard::get_keycode(CL_KEY_ESCAPE); 

	if (readyforkey && pause) {
		if (state == PAUSED && pausemenu->type == Menu::INGAME) {
			state = PLAYING;
			cltime = CL_System::get_time();
			if (globals->verbosity < 0)
				cout << "Resynced time: " << cltime << endl;
		} else { 
			state = PAUSED;
			pausetimer = CL_System::get_time();
			delay = 0;	// doesn't timeout
			pausemenu->status = Menu::NONE;
		}
	}
	readyforkey = !pause;
}

bool World::listenAnyKey() {
	if (globals->buffer->keys_left()) {
		globals->buffer->clear();
		return true;
	}
	return false;
}
