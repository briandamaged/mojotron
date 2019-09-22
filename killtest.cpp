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
#include "Hazards.hh"
#include "Barricade.hh"
#include "Globals.hh"
#include "Bonus.hh"
#include "ScoreBoard.hh"
#include "MiscDisplay.hh"
#include "ObjectList.hh"

using namespace std;
World* worldobj;

World::World() {
	// General variables, global speed etc.
	orig_monster_speed = (float)Globals::loadInt("Constants/monsterspeed")/100;
	monster_size = 0;	// zero is no adjustment
	deltamodify = Globals::skilllevelspeed[Globals::difficulty];

	num_players = Globals::num_players;
	monster_quantity = (float)Globals::skilllevelquantity[Globals::difficulty]/100;
	monster_quantity *= (1.0f + (float)(num_players - 1) * 0.5f);

	lives = num_players*Globals::loadInt("PlayerSpecs/startlives");
	all = new ObjectList(1024);

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

	spr_grunt = 0;
	base = 0;
	flashing = 0;	gameovertime = 0;
}

World::~World() {
	Thing* temp;
	while ((temp = all->getNext())) {
		if (Globals::verbosity > 1)
			cout << "Deleting Thing: " << temp << endl;
		delete temp;
	}
	delete all;
	delete elist;
	delete ellist;
	delete pllist;
	delete olist;
	delete ollist;

	delete base;
	
	if (!Globals::loadInt("Testing/singlegruntsprite")) {
		// will be null if world deleted immediately
		// I guess deleting null pointers is harmless
		delete spr_grunt;
	}
	delete miscdisplay;
	worldobj = 0;
}

/*
 * LEVEL LOADING
 ****************/

void World::load_level(int l) {
	try {
		num_lev = 12;
		consoletimer = 0;

		// make char array string containing level num. extension
		char number[3];
		sprintf(number, "%d", l);
		
		// put the directory for the current level in string 'base'
		if (base)	delete base;
		base = new std::string("Levels/level/");
		base->insert((unsigned int)12, number);
		if (Globals::verbosity > 0)
			cout << "Loading: " << *base << endl;
		
		lev_name = loadlevString("lev_name");
		/*warp_time = loadlevInt("warp_time");
		warp_time += (int)((monster_quantity - 1.0f) * warp_time / 2);
		warp_time /= num_players;*/
		warp_time = 0;

		// increase the player's stats
		for (int i=0; i < num_players; i++) {
			playerlist[i]->speedUp(num_lev / 10);
		}
		// increase the monsters stats
		monster_speed = orig_monster_speed + (float)num_lev/100;

		bgred = loadlevInt("bg_red");
		bggreen = loadlevInt("bg_green");
		bgblue = loadlevInt("bg_blue");
		
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
		if (Globals::loadInt("Testing/singlegruntsprite")) {
			spr_grunt = Globals::spr[Globals::CRATE];
		} else {
			if (spr_grunt) {
				delete spr_grunt;
			}
			std::string temp = std::string("Surfaces/");
			temp.append(loadlevString("Grunt"));
			spr_grunt = new Sprite(temp, Globals::manager);
		}
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

		// monsters required killed
		for(n=0; n < (int)(monster_quantity * num_balls); n++) {
			if (Globals::verbosity > 1) cout << "z";
			all->addThing(new Ball(Globals::spr[Globals::BALL]));
		}
		for(n=0; n < (int)(monster_quantity * num_grunts); n++) {
			if (Globals::verbosity > 1) cout << "y";
			Thing* temp = new Grunt(spr_grunt, gruntspeed);
			if (grunthealth) temp->health = grunthealth;
			if (barricadeson) temp->dropsbarricade = true;
			all->addThing(temp);
		}
		Grunt::escapepod = gruntescapepod;
		Grunt::randompod = gruntrandompod;
		Grunt::bouncingpod = gruntbouncingpod;
		for(n=0; n < (int)(monster_quantity * num_spikers); n++) {
			if (Globals::verbosity > 1) cout << "s";
			all->addThing(new Spiker(Globals::spr[Globals::SPIKER]));
		}
		for(n=0; n < (int)(monster_quantity * num_asteroids); n++) {
			if (Globals::verbosity > 1) cout << "a";
			all->addThing(new Asteroid());
		}
		for(n=0; n < (int)(monster_quantity * num_snipers); n++) {
			if (Globals::verbosity > 1) cout << "g";
			all->addThing(new Sniper(Globals::spr[Globals::SNIPER]));
		}
		for(n=0; n < (int)(monster_quantity * num_rightanglers); n++) {
			if (Globals::verbosity > 1) cout << "g";
			if (rightanglermix == 1) {
				all->addThing(new RightAngler(Globals::spr[Globals::RIGHTANGLER], true));
			} else if (rightanglermix == 2) {
				all->addThing(new RightAngler(Globals::spr[Globals::RIGHTANGLER], false));
			} else {
				all->addThing(new RightAngler(Globals::spr[Globals::RIGHTANGLER], (rand() < RAND_MAX/2)));
			}
		}
		for(n=0; n < (int)(monster_quantity * num_turrets); n++) {
			all->addThing(new Turret(Globals::spr[Globals::TURRET], turretfansize));
		}
		for(n=0; n < (int)(monster_quantity * num_soldiers); n++) {
			Thing* temp = new Soldier(Globals::spr[Globals::SOLDIER]);
			if (barricadeson) temp->dropsbarricade = true;
			all->addThing(temp);
		}
		for(n=0; n < (int)(monster_quantity * num_simpleboss); n++) {
			all->addThing(new SimpleBoss(Globals::spr[Globals::BOSS], n));
		}
		for(n=0; n < (int)(monster_quantity * num_circleboss); n++) {
			// evenly distribute the bosses around their circles, and make one circle 180deg to the other
			all->addThing(new CircleBoss(Globals::spr[Globals::BOSS], 
							ARENAWIDTH/3 * (1 + n%2), ARENAHEIGHT/2, gruntspeed, 
							(n/2)*4*M_PI/num_circleboss + (1 + n%2)*M_PI)); 
		}
		for(n=0; n < (int)(monster_quantity * num_centipedes); n++) {
			Thing* head = new Centipede(Globals::spr[Globals::CENTIHEAD], 0);
			all->addThing(head);

			Thing* leadby = head;
			for(int j=1; j < centipedelength; j++) {
				Thing* temp = new Centipede(Globals::spr[Globals::CENTISEG], leadby);
				all->addThing(temp);
				leadby = temp;
			}
			head->startPos();	// special case. Other monsters should call their own startPos
		}
		Thing* previous = 0;
		for(n=0; n < (int)(monster_quantity * num_cellphones); n++) {
			Thing* temp = new Cellphone(Globals::spr[Globals::CELLPHONE],
							previous);
			all->addThing(temp);
			previous = temp;
		}
		for(n=0; n < (int)(monster_quantity * num_spirals); n++) {
			all->addThing(new Spiral(Globals::spr[Globals::SPIRAL]));
		}
		max_active = num_active = all->getNumToBeKilled();

		// optionally killed
		for(n=0; n < (int)(monster_quantity * num_obst); n++) {
			if (Globals::verbosity > 1) cout << "x";
			all->addThing(new Obstacle(Globals::spr[Globals::OBSTACLE]));
		}
		for(n=0; n < num_fruit; n++) {
			if (Globals::verbosity > 1) cout << "f";
			all->addThing(new Fruit(fruittypes));
		}
		for(n=0; n < (int)(monster_quantity * num_rockmen); n++) {
			if (Globals::verbosity > 1) cout << "r";
			all->addThing(new RockMan(Globals::spr[Globals::ROCKMAN]));
		}
		for(n=0; n < num_barricades; n++) {
			all->addThing(new Barricade());
		}
		for(n=0; n < (int)(monster_quantity * num_lasers); n++) {
			if (rand() > RAND_MAX/2)
				all->addThing(new Laser(Globals::spr[Globals::LASERVERT], true));
			else
				all->addThing(new Laser(Globals::spr[Globals::LASERHORIZ], false));
		}
		
		for(int i=0; i < num_players; i++) {
			playerlist[i]->startPos();
		}

	} catch(CL_Error err) {
		cout << "CL_Error: " << err.message.c_str() << std::endl;
	}
}

// templates would probably be the good thing here

int World::loadlevInt(std::string dataname) {
	// loads /Levels/levelxx/dataname, from the resource file
	std::string temp = *base;
	temp.append(dataname);
	return (Globals::loadInt(temp));
}

std::string World::loadlevString(std::string dataname) {
	// loads /Levels/levelxx/dataname, from the resource file
	std::string temp = *base;
	temp.append(dataname);
	return (Globals::loadString(temp));
}

/*CL_SurfaceProvider* World::loadlevSurface(std::string dataname) {
	// loads /Levels/levelxx/dataname, from the resource file
	std::string temp = *base;
	temp.append(dataname);
	return (Globals::loadSurface(temp));
}*/

/*
 * DRAWING
 **********/

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
	// draw playfield unless we've faded out
	if ( gameovertime < 3000 ) {	
		drawBG(delta);
		all->toListStart();	Thing* x;
		while ((x = all->getNext())) {
			x->draw();
		}
	}

	if (state != GAMEOVER) {
		/*for (int i=0; i < num_players; i++) {
			int xstart = (i%2)*SECONDXORIGIN + PAD;
			int ystart = (i/2)*BOARDHEIGHT + (PAD+(ARENAHEIGHT>>8));
			playerlist[i]->scoreboard->drawScoreBoard( xstart, ystart );
		}*/
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
				// TODO: stop this from lying with updated rules
				miscdisplay->drawLevWarp(1, 3);
				break;
			}
			case ENDGAME: {
				Globals::largefont->print_center(XWINSIZE>>9, (YWINSIZE>>9)-100, "404 No More Game!!");
				Globals::smallfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)-50, "This is the file not found message of GLORY.");
				Globals::smallfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)-25, "Mojotron can't find another level, so you win!");
				Globals::smallfont->print_center(XWINSIZE>>9, (YWINSIZE>>9), "As a prize, you will receive your very own homicidal");
				Globals::smallfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)+25, "monkey in the post.");
				Globals::smallfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)+75, "We suggest you start running NOW!!");
				Globals::smallfont->print_center(XWINSIZE>>9, (YWINSIZE>>9)+150, "Press any key to continue.");
				break;
			}
			case PAUSED: {
				Globals::largefont->print_center(XWINSIZE>>9, YWINSIZE>>9, "Paused");
				break;
			}
			case READY: {
				Globals::largefont->print_center(XWINSIZE>>9, YWINSIZE>>9, "Ready...");
				break;
			}
			case PLAYING:
			case DIED:
			case WON:
			case WARPED:
			case GAMEOVER:
			case EXIT:
			case DELAY:
				;
		}

	} else {
		string endmessage;
		for (int i=0; i < num_players; i++) {
			//endmessage += playerstatslist[i]->report();
			// CHANGES NEEDED
			endmessage += playerlist[i]->stats->report();
		}
		int pos = 0;
		if (gameovertime > 3000) {	// scrolling
			pos = (gameovertime - 3000)/20;
			CL_Display::clear_display(0, 0, 0, 1.0f);
			Globals::smallfont->print_left(40, 500-pos, endmessage.c_str());
		} else {
			float alpha = (float)(gameovertime)/3000;
			CL_Display::clear_display(0, 0, 0, alpha);
		}
		Globals::largefont->print_center(XWINSIZE>>9, 200-pos, "GAME OVER");
				
	}

	CL_Display::flip_display();
}

/*
 * GAME LOOP
 ************/

// sends back the number of the next level, or 0 if quit or ran out of lives
int World::run() {
	cltime = CL_System::get_time();	// don't get the time directly, use this
	oldtime = CL_System::get_time();
	inittime = oldtime;
	int maxdelta = Globals::loadInt("Constants/maxdelta");
	
	levelage = delta = 0;
	unsigned long framenum = 0;
	
	delay = 0;
	keypcount = 0;
	
	if 	(lev_name == "") state = ENDGAME;
	else 	gameDelay(	Globals::loadInt("Constants/readypause"),
			READY, PLAYING );
	
	char levchars[10];
	sprintf(levchars, "Level %d: ", num_lev);
	std::string levstring = std::string(levchars);
	miscdisplay->setConsole(levstring.append(lev_name), Globals::loadInt("Constants/levelinfotimeout"));
	Globals::playSound(Globals::BEGINLEV);

	/*int fsk=0, failcount=0;
	static int fs[12]={ 1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 20, 24 };*/
				for(int i=0; i < num_players; i++) {
					playerlist[i]->invincible = true;
				}
	
	while 	((CL_Keyboard::get_keycode(CL_KEY_ESCAPE) == false)) 
	{
		delta = 0;

		switch(state) {
			case PLAYING: {
				// don't want time updated while paused
				cltime = CL_System::get_time();
				delta = cltime - oldtime;
				delta = delta*deltamodify / 100;

				// if we get an excessivly large delta, don't act on it.
				if (delta > maxdelta)	delta = 0;
				
				listenPauseToggle();	
				
				if (!num_players) state = GAMEOVER;
			
				Grunt::calcSpeed(delta, max_active-num_active);
				
				// move all
				all->toListStart();	Thing* x;
				while ((x = all->getNext())) {
					x->hurt(10);
					x->move(delta);
				}
				all->clearDeadStuff();
				if (!num_active) {
					all->addThing(new Asteroid());
				}
				if (num_active < 0)	state = WON;
	
				// Check for collisions, and call event methods
				makeLists();
				findEnemyCols();
				findPlayerCols();
				findObstacleCols();

				break;
			}
		
			case WON: {
				Globals::playSound(Globals::LEVELOVER);
				for(int i=0; i < num_players; i++) {
					// CHANGES NEEDED
					playerlist[i]->stats->wonLevel(getLevelAge()/1000, warp_time);
				}
				if (warp_time > (getLevelAge()/1000)) {
					gameDelay(3000, World::WARPING, World::WARPED);
					lives++;
				} else
					return (num_lev + 1);
				break;
			}

			case WARPED: {
				return (num_lev + Globals::loadInt("Constants/levelskip"));
			}

			case GAMEOVER: {
				gameovertime = CL_System::get_time()-cltime;

				if (listenAnyKey()) {
					// ignore it for the first moments
					if (gameovertime > 2000) {
						return 0;
					}
				}
				break;
			}

			case EXIT: {
				return 0;
			}

			case ENDGAME: {
				if (listenAnyKey()) {
					return 0;
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
			case DELAY: {
				maintainDelay();
			}
		}

		// crude profiling stuff
		// int gamelogictime = CL_System::get_time() - cltime;
		
		updateDisplay();
		
		//int graphicstime = (CL_System::get_time() - cltime) - gamelogictime;
		
		CL_System::keep_alive();

		levelage += delta;
		oldtime = cltime;
		framenum++;

		if (Globals::verbosity > 1) {
			cout << "num_active = " << num_active << "\tdelta = " << delta << endl;
			//cout << "Logic took: " << gamelogictime << "\tGraphics took: " << graphicstime << endl;
		}
	} // end of no escape pressed while loop

	cout << endl << endl << "Average rate: " << 
		1000 * framenum / (CL_System::get_time() - inittime) << "fps" << endl;

	return 0;
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
	if ((CL_System::get_time()-cltime) > delay) {
		state = oldstate;
		cltime = CL_System::get_time();
	}
}

void World::restart() {
	oldtime = CL_System::get_time(); // avoid a large delta being made
	
	all->toListStart();	Thing* x;
	while ((x = all->getNext())) {
		x->startPos();
	}
	
	// slow down the grunts a bit, give them a chance
	Grunt::agespeedup -= Globals::loadInt("Constants/playerdeathslowdown");
	if (Grunt::agespeedup < 0) Grunt::agespeedup = 0;

	if (Globals::verbosity > 0) {
		cout << "Grunt speed status:" << endl;
		cout << "Age increase = " << Grunt::agespeedup << endl << "Low numbers increase = " << Grunt::numspeedup << endl;
	}

	gameDelay( Globals::loadInt("Constants/readypause"), READY, PLAYING);
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

	defender = (int)sqrt(	pow((playerlist[0]->xpos)-xpos, 2) + 
				pow((playerlist[0]->ypos)-ypos, 2));
	bestcandidate = playerlist[0];
	for (int i=1; i < num_players; i++) {
		challenger = (int)sqrt(	pow((playerlist[i]->xpos)-xpos, 2) + 
					pow((playerlist[i]->ypos)-ypos, 2));
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
	Player* newplay = new Player(Globals::spr_player[i], i);
	playerlist[i] = newplay;
	all->addThing(newplay);
	newplay->playing = true;
}


/*
 * MISC INPUT STUFF
 *******************/

void World::listenPauseToggle() {
	CL_Key bufferkey;

	while(Globals::buffer->keys_left() != 0) {
		bufferkey = Globals::buffer->get_key();
		if (bufferkey.id == CL_KEY_P) keypcount++;
		//cout << bufferkey.id;
	}
	
	if (keypcount && !(keypcount%2)) {	// there's a signal for both press and release
		keypcount = 0;
		if (state == PAUSED) {
			state = PLAYING;
			cltime = CL_System::get_time();
			if (Globals::verbosity < 0)
				cout << "Resynced time: " << cltime << endl;
		} else { 
			state = PAUSED;
		}
	}
}

bool World::listenAnyKey() {
	if (Globals::buffer->keys_left()) {
		Globals::buffer->clear();
		return true;
	}
	return false;
}
