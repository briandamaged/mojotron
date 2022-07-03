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

#include "Sound.hh"
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

Sound* Sound::soundobj = NULL;
bool Sound::sound = false;
bool Sound::music = false;

Sound::Sound(string resfile) {
	string mus_resname[] = {
		"Music/title",
		"Music/game0"
	};

	FILE *f = fopen(resfile.c_str(), "rb");
	IniFile sndmanager;
	int namestart = strlen("Sounds/");

	sndmanager.read(f);
	fclose(f);
	// For each sample
	std::unique_ptr<list<string> > samps = sndmanager.get_resources_of_type(IniElement::sample);
        list<string>::iterator first = samps->begin();
        list<string>::iterator last = samps->end();
	while (first != last) {
		// resname is of the form "Sounds/foobarX"
		// name is of the form "foobar"
		string resname = *first;
		int nameend = resname.size()-1;
		string name = resname.substr(namestart, nameend-namestart);

		Mix_Chunk *sb = Mix_LoadWAV(sndmanager.get((*first).c_str(), std::string("")).c_str());

		// do these sfx have polyphony?
		if (!polyphony.count(name))
			initPolyphony(resname, name, sndmanager);

		sfx.insert(pair<string, Mix_Chunk *>(name, sb));
/*		playing.insert(pair<string, CL_SoundBuffer_Session>
			(name, sb.prepare()));*/
		first++;
	}

	int i;
	for(i=0; i < Sound::MUSEND; i++) {
		mus[i] = Mix_LoadMUS(sndmanager.get(mus_resname[i].c_str(), std::string("")).c_str());
	}

}

void Sound::initPolyphony(string resname, string basename, IniFile &ini) {
	bool poly = false;
	poly = (bool)ini.get(resname + "polyphony", 0);

	polyphony.insert(pair<string, bool>(basename, poly));
}

Sound::~Sound() {
	stopMusic();

//	delete sndmanager;
}

/* Static interface methods */

void Sound::setSFX(bool on) {
	sound = on;
	initAudio();
}

void Sound::setMusic(bool on) {
	if (on) {
		music = true;
		initAudio();
	} else {
		if (soundobj) {
			soundobj->stopMusic();
			music = false;
		}
	}
}

void Sound::initAudio() {
	// don't init audio unless we're going to use it
	if (!(sound || music))	return;
	// check if we've already ran initAudio
	if (soundobj)		return;

	Mix_Init(MIX_INIT_FLAC | MIX_INIT_OGG | MIX_INIT_MOD);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);

	soundobj = new Sound();
}

void Sound::deinitAudio() {
	if (soundobj) {
		delete soundobj;
		soundobj = NULL;

		Mix_Quit();
	}
}

void Sound::playSound(string resname, float pan) {
	if (sound) {
		// Figure out which variant of the sound effect to play
		int variants = soundobj->sfx.count(resname);
		int number = (int)(variants * ((float)rand() / RAND_MAX));
		if (!variants) cout << "Error: Sample " << 
			resname << " not found." << endl;

		// Find where the samples are
		multimap<string, Mix_Chunk*>::iterator iter = 
			soundobj->sfx.find(resname);

		// Find the one we need
		// only a bidirectional iterator, loop instead of iter+=number
		for (; number > 0; number--) iter++;
		Mix_Chunk *sb = (*iter).second;

		// If it's already playing, do we need to play it again?
		if (!soundobj->canPlaySound(resname)) {
			//cout << "Sample already playing" << endl;
			return;
		}

		// Play the sample
		int ch = Mix_PlayChannel(-1, sb, 0);
		soundobj->playing[ch] = resname;
/*		try {
			CL_SoundBuffer_Session s = sb.play();
			s.set_pan(pan);

			// soundbuffer sessions are only 8 bytes, it's not
			// that expensive to throw them around by value.
			(*soundobj->playing.find(resname)).second = s;
		} catch(CL_Error err) {
			cout << err.message.c_str() << endl;
		}*/
	}
}

bool Sound::canPlaySound(string resname) {
	bool playmany = (*soundobj->polyphony.find(resname)).second;
	if (playmany)	return true; // polyphony's on, play another instance

	for (int i = 0; i < NUM_CHANNELS; i++)
	{
		if (playing[i] == resname)
		{
			if (0 == Mix_Playing(i))
			{
				playing[i] = "";
				return true;
			}
			else
				return false;
		}
	}
	return true;
}

void Sound::playMusic(musindex number) {
	if (music) {
		Mix_HaltMusic();
		Mix_PlayMusic(soundobj->mus[number], -1);
	}
}

void Sound::stopMusic() {
	if (soundobj) {
		Mix_HaltMusic();
	}
}
