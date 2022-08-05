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

#include "Application.h"
#include <SDL.h>
#include "World.hh"
#include "Intro.hh"
#include "Sound.hh"
#include "SkillLevel.hh"
#include "Display.hh"
#include "Demo.hh"
#include "InputState.hh"
#include "ConfigFile.hh"
#include "PlayerStats.hh"
#include "IRandom.hh"
#include <iostream>

extern World* worldobj;
extern Globals* globals;

SDL_Renderer *game_renderer;

Application *Application::app;

class MojoApp : Application {
	private:
		ConfigFile* configf;
		Config conf;
		SDL_Window *game_window;
		std::unique_ptr<Demo> demo;
		std::unique_ptr<Intro> intro;

	public:

	const char* get_title() {
		return "Mojotron";
	}

	void quit() {
		if (worldobj) delete worldobj;

		configf->saveSettings(configf->getCurrentSettings());
		delete configf;
		configf = NULL;

		intro = NULL;
		Sound::deinitAudio();
		SkillLevel::deinitAll();
		InputState::deinitControls();
		delete globals;
		globals = NULL;

		SDL_Quit();
		exit(EXIT_SUCCESS);
	}
	
	int main(int argc, char** argv) {
		bool demotest, recorddemo;
		bool fullscreen, sound, music, cmdlineoptsset;
		demotest = recorddemo = false;
		fullscreen = sound = music = cmdlineoptsset = false;
		std::string resourcefile = "robotwar.xml";
		std::string demofile = "demo";
		int verbosity = 0;

		for (int i=1; i < argc; i++) {
			if (!strcmp(argv[i], "--fullscreen")) {
				fullscreen = true;
				cmdlineoptsset = true;
			}
			if (!strcmp(argv[i], "--enable-sound")) {
				sound = true;
				cmdlineoptsset = true;
			}
			if (!strcmp(argv[i], "--disable-sound")) {
				sound = false;
				cmdlineoptsset = true;
			}
			if (!strcmp(argv[i], "--enable-music")) {
				music = true;
				cmdlineoptsset = true;
			}
			if (!strcmp(argv[i], "--disable-music")) {
				music = false;
				cmdlineoptsset = true;
			}
			if (!strcmp(argv[i], "--record-demo")) {
				recorddemo = true;
			}
			if (!strcmp(argv[i], "-v")) {
				verbosity = 1;
			}
			if (!strcmp(argv[i], "-f")) {
				cout << "Loading resourcefile " << argv[i+1] << endl;
				resourcefile =  argv[i+1];
				i++;
			}
			if (!strcmp(argv[i], "-d")) {
				demofile = argv[i+1];
				i++;
			}
		}

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0)
		{
			std::cout << "Failed - SDL_Init" << std::endl;
			exit(0);
		}

		configf = new ConfigFile();
		conf = configf->getFileSettings();

		if (!cmdlineoptsset) {
			fullscreen = conf.fullscreen;
			music = conf.music;
			sound = conf.sound;
		}
		Sound::setMusic(music);
		Sound::setSFX(sound);
		PlayerStats::enterScore(conf.highscore);

		game_window = SDL_CreateWindow(get_title(),
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			XWINSIZE >> 8, YWINSIZE >> 8,
			(fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
		if (game_window == NULL)
		{
			std::cout << "Failed - SDL_CreateWindow" << std::endl;
			exit(0);
		}

		game_renderer = SDL_CreateRenderer(game_window, -1, 0);
		if (game_renderer == NULL)
		{
			std::cout << "Failed - SDL_CreateRenderer" << std::endl;
			exit(0);
		}
		SDL_RenderSetLogicalSize(game_renderer, XWINSIZE >> 8, YWINSIZE >> 8);

		globals = new Globals(resourcefile);
		globals->loadSprites();
		SkillLevel::initAll();
		SkillLevel::setSkill(conf.skilllevel);
		InputState::initControls(conf);
		globals->fullscreen = fullscreen;
		globals->verbosity = verbosity;

		IRandom::init();

		bool play;
		if (recorddemo)
			demo = std::make_unique<Demo>();
		else
			demo = std::make_unique<Demo>(demofile);
		intro = std::make_unique<Intro>(demo.get());

		if (intro->show()) { // run intro, menus etc.
			do {
				Sound::playMusic(Sound::GAME0);
				worldobj = new World();
				int level; level = 1;
				do {
					worldobj->load_level(level);
					level = worldobj->run(recorddemo);
				} while(level != 0);
		
				if (globals->verbosity > 0)	
					cout << "Deleting world from main" << endl;
				delete worldobj;	// nulls itself out
				
				play = intro->show();
			} while (play);
		}
		quit();
		return (EXIT_FAILURE);
	}
} app;

int main(int argc, char *argv[])
{
	return app.main(argc, argv);
}
