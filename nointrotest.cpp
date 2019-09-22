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
//#include <ClanLib/display.h>
#include "World.hh"
#include "Thing.hh"
#include "Globals.hh"
#include "Intro.hh"

extern World* worldobj;

class MojoApp : public CL_ClanApplication {
	public:

	// Clanlib 0.4 compatibility (is shagged anyways)
	/*void init_modules() { }
	void deinit_modules() { }*/
	
	virtual char* get_title() {
		return "Mojotron";
	}
	
	virtual int main(int argc, char** argv) {
		try {
			CL_SetupCore::init();
			//CL_SetupCore::init_display();
			CL_SetupDisplay::init();

			bool fullscreen = false;
			std::string resourcefile = "res.scr";
			for (int i=1; i < argc; i++) {
				if (!strcmp(argv[i], "--fullscreen")) {
					fullscreen = true;
				}
				if (!strcmp(argv[i], "--enable-sound")) {
					Globals::sound = true;
				}
				if (!strcmp(argv[i], "--disable-sound")) {
					Globals::sound = false;
				}
				if (!strcmp(argv[i], "-v")) {
					Globals::verbosity = 2;
				}
				if (!strcmp(argv[i], "-f")) {
					cout << "Loading resourcefile " << argv[i+1] << endl;
					resourcefile =  argv[i+1];
					i++;
				}
			}

			if (Globals::sound)
				CL_SetupSound::init();

			CL_Display::set_videomode(XWINSIZE >> 8, YWINSIZE >> 8, 16, fullscreen);

			srand((int)CL_System::get_time());
	
			Globals::loadAll(resourcefile);
		
				// endless loop, do not fullscreen!
				do {
					worldobj = new World();
					int level; level = 1;
					do {
						worldobj->load_level(level);
						level = worldobj->run();
					} while(level != 0);
			
					if (Globals::verbosity > 0)
						cout << "Deleting world from main" << endl;
					delete worldobj;	// nulls itself out
				} while(true);

			if (Globals::verbosity > 0)
				cout << "deinitted intro" << endl;
			Globals::deinit();
			if (Globals::verbosity > 0)
				cout << "deinitted globals" << endl;

			CL_SetupCore::deinit();
			//CL_SetupCore::deinit_display();
			CL_SetupDisplay::deinit();

			if (Globals::sound)
				CL_SetupSound::deinit();

		// catch for the whole thing!!?
		} catch (CL_Error err) {
			cout << err.message.c_str() << endl;
		}
		return 0;
	}
} app;

