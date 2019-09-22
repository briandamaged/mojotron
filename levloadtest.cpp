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
#include <ClanLib/mikmod.h>
#include "World.hh"
#include "Intro.hh"
#include "Sound.hh"
#include "Sprite.hh"
#include "SkillLevel.hh"
#include "MiscDisplay.hh"
#include "InputState.hh"
#include "Display.hh"

extern World* worldobj;
extern Globals* globals;

class MojoApp : public CL_ClanApplication {
	public:

	virtual char* get_title() {
		return "Mojotron";
	}
	
	virtual int main(int argc, char** argv) {
		try {
			CL_SetupCore::init();
			CL_SetupDisplay::init();

			std::string resourcefile = "empty.scr";
			bool fullscreen = false;
			int verbosity = 0;

			for (int i=1; i < argc; i++) {
				/*if (!strcmp(argv[i], "--fullscreen")) {
					fullscreen = true;
				}*/
				if (!strcmp(argv[i], "--enable-sound")) {
					Sound::setSFX(true);
				}
				if (!strcmp(argv[i], "--disable-sound")) {
					Sound::setSFX(false);
				}
				if (!strcmp(argv[i], "--enable-music")) {
					Sound::setMusic(true);
				}
				if (!strcmp(argv[i], "--disable-music")) {
					Sound::setMusic(false);
				}
				if (!strcmp(argv[i], "-v")) {
					verbosity = 1;
				}
				if (!strcmp(argv[i], "-f")) {
					cout << "Loading resourcefile " << argv[i+1] << endl;
					resourcefile =  argv[i+1];
					i++;
				}
			}
			CL_Display::set_videomode(XWINSIZE >> 8, YWINSIZE >> 8, 16, globals->fullscreen);

			globals = new Globals(resourcefile);
			SkillLevel::initAll();
			InputState::initControls();
			globals->fullscreen = fullscreen;
			globals->verbosity = verbosity;

			srand((int)CL_System::get_time());
	
			bool play;
		
			if (Intro::show()) { // run intro
				do {
					Sound::playMusic(Sound::GAME0);
					worldobj = new World();
					int level; level = 1;
					// endless
					do {
						worldobj->load_level(level);
						level = worldobj->run(false);
					} while(true);
			
					if (globals->verbosity > 0)	
						cout << "Deleting world from main" << endl;
					delete worldobj;	// nulls itself out
					
					play = Intro::show();
				} while (play);
			}

			Intro::deinit();
			Sound::deinitAudio();
			SkillLevel::deinitAll();
			delete globals;
			globals = NULL;

			CL_SetupCore::deinit();
			CL_SetupDisplay::deinit();

		// catch for the whole thing!!?
		} catch (CL_Error err) {
			cout << err.message.c_str() << endl;
		}
		return (EXIT_SUCCESS);
	}
} app;

