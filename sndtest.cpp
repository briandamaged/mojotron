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
#include <ClanLib/sound.h>
#include <ClanLib/mikmod.h>
#include "Sound.hh"

class MojoApp : public CL_ClanApplication {
	public:

	virtual char* get_title() {
		return "Mojotron";
	}
	
	virtual int main(int argc, char** argv) {
		try {
			CL_SetupCore::init();

			for (int runs=0; runs < 1; runs++) {
				cout << "Making a sound instance" << endl;
				Sound::setSFX(true);

				for (int j=0; j < 200; j++) {
					for (int i=0; i < 5; i++)
						//Sound::playSound("die");
						Sound::playSound("explode");	// stress test
					CL_System::sleep(100);
				}

				cout << "Destroying instance" << endl;
				Sound::deinitAudio();
				cout << "Looping" << endl;
			}

			CL_SetupCore::deinit();

		} catch (CL_Error err) {
			cout << err.message.c_str() << endl;
		}
		return 0;
	}
} app;

