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
#include <ClanLib/application.h>
#include "World.hh"
#include "Intro.hh"
#include "Sound.hh"
#include "SkillLevel.hh"
#include "Display.hh"
#include "Demo.hh"
#include "InputState.hh"
#include "ConfigFile.hh"
#include "PlayerStats.hh"

#ifdef HAVEMIKMOD
#include <ClanLib/mikmod.h>
#endif

extern World* worldobj;
extern Globals* globals;

class MojoApp : public CL_ClanApplication {
	private:
		ConfigFile* configf;
		Config conf;
	public:

	char* get_title() {
		return "Mojotron";
	}

	void quit() {
		if (worldobj) delete worldobj;

		configf->saveSettings(configf->getCurrentSettings());
		delete configf;
		configf = NULL;

		Intro::deinit();
		Sound::deinitAudio();
		SkillLevel::deinitAll();
		InputState::deinitControls();
		delete globals;
		globals = NULL;

		CL_SetupCore::deinit();
		CL_SetupDisplay::deinit();
		exit(EXIT_SUCCESS);
	}
	
	int main(int argc, char** argv) {
		bool demotest, recorddemo;
		bool fullscreen, sound, music, cmdlineoptsset;
		demotest = recorddemo = false;
		fullscreen = sound = music = cmdlineoptsset = false;
		std::string resourcefile = "res.scr";
		std::string demofile = "";
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
				demotest = true;
				i++;
			}
		}

		try {
			CL_SetupCore::init();
			CL_SetupDisplay::init();

#ifdef WIN32
			if (verbosity) {
				CL_ConsoleWindow console("My console");
				console.redirect_stdio();
				cout << "Debug console up" << endl;
			}
#endif

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

			CL_Display::set_videomode(XWINSIZE >> 8, YWINSIZE >> 8, 16, fullscreen);
			CL_Slot windowclose = CL_System::sig_quit().connect(this, &MojoApp::quit);

			globals = new Globals(resourcefile);
			globals->loadSprites();
			SkillLevel::initAll();
			SkillLevel::setSkill(conf.skilllevel);
			InputState::initControls(conf);
			globals->fullscreen = fullscreen;
			globals->verbosity = verbosity;

			srand((int)CL_System::get_time());
	
			if (demotest) {
				Demo d = Demo(demofile);
				d.test();
			}

			bool play;
		
			if (Intro::show()) { // run intro, menus etc.
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
					
					play = Intro::show();
				} while (play);
			}
			quit();
		} catch (CL_Error err) {
			cout << err.message.c_str() << endl;
		}
		// we caught something, otherwise we'd left through quit
		return (EXIT_FAILURE);
	}
} app;

