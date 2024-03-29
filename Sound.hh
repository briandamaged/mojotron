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

#ifndef SOUND_HH
#define SOUND_HH

#include <SDL_mixer.h>
#include <string>
#include <map>
#include "IniFile.h"
using namespace std;

#define SNDRESFILE "sound.scr"
#define NUM_CHANNELS 20

struct Soundeffect;

class Sound {
public:
	enum musindex {
		TITLE,
		GAME0,
		MUSEND
	};

	static void setSFX(bool on);
	static void setMusic(bool on);
	static bool sound;
	static bool demo;
	static bool music;

	/* makes a Sound instance and keeps it to itself in 
	 * Singleton fashion
	 */
	static void initAudio();
	static void deinitAudio();

	static void playSound(std::string name, float pan=0);
	static void playMusic(musindex number);
	static void stopMusic();

private:
	static Sound* soundobj;

	Sound(string resfile = SNDRESFILE);
	~Sound();
	void initPolyphony(string resname, string basename, IniFile &ini);
	bool canPlaySound(string resname);

	// look these up by effect name, e.g. "explode"
	std::multimap<std::string, Mix_Chunk*> sfx;
	std::map<std::string, bool> polyphony;
	std::string playing[NUM_CHANNELS];

	Mix_Music* mus[MUSEND];
};

#endif

