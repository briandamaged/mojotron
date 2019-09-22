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
#include "Globals.hh"
#include "Sprite.hh"

#ifdef HAVEMIKMOD
#include <ClanLib/mikmod.h>
#endif

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

			std::string resourcefile = "res.scr";
			for (int i=1; i < argc; i++) {
				if (!strcmp(argv[i], "-f")) {
					cout << "Loading resourcefile " << argv[i+1] << endl;
					resourcefile =  argv[i+1];
					i++;
				}
			}
			//CL_Display::set_videomode(640, 480, 16, false);

			int x = 0;
			while (!CL_Keyboard::get_keycode(CL_KEY_ESCAPE)) {
				cout << "Top of loop, " << x << " inits." << endl;
				globals = new Globals(resourcefile);	x++;
				delete globals;
				cout << "Did gratuitous init/deinit " << x << " inits." << endl;
				globals = new Globals(resourcefile);	x++;

				for (int i=0; i < 100; i++) {
					cout << "lives = " << globals->loadInt("PlayerSpecs/startlives") << endl;
					cout << "nonexistant = " << globals->loadInt("foo") << endl;
				}
				//Globals::playSound(Globals::HURT);
				
				CL_System::keep_alive();
				CL_System::sleep(200);
				
				delete globals;
			}

			CL_SetupCore::deinit();
			CL_SetupDisplay::deinit();

		// catch for the whole thing!!?
		} catch (CL_Error err) {
			cout << err.message.c_str() << endl;
		}
		return 0;
	}
} app;

