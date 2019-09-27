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

/*	sndmanager = new CL_ResourceManager(resfile, false);
	int namestart = strlen("Sounds/");
        
	// For each sample
	list<string>* samps = sndmanager->get_resources_of_type("sample");
        list<string>::iterator first = samps->begin();
        list<string>::iterator last = samps->end();
	while (first != last) {
		// resname is of the form "Sounds/foobarX"
		// name is of the form "foobar"
		string resname = *first;
		int nameend = resname.size()-1;
		string name = resname.substr(namestart, nameend-namestart);
		
		CL_SoundBuffer sb = CL_SoundBuffer((*first).c_str(),
			sndmanager);

		// do these sfx have polyphony?
		if (!polyphony.count(name))
			initPolyphony(resname, name);
		initSFXParams(resname, sb);

		sfx.insert(pair<string, CL_SoundBuffer>(name, sb));
		playing.insert(pair<string, CL_SoundBuffer_Session>
			(name, sb.prepare()));
		first++;
	}
	delete samps;*/

#ifdef HAVEMIKMOD
	int i;
	for(i=0; i < Sound::MUSEND; i++) {
		mus[i] = CL_Streamed_MikModSample::load(mus_resname[i].c_str(), sndmanager, true);
		//mus[i]->set_volume(1.0f);
	}
#endif

}

/*void Sound::initSFXParams(string resname, CL_SoundBuffer sb) {
	try {
		int volume = CL_Integer(resname + "vol", sndmanager);
		bool status = sb.set_volume((float)volume/100);
		if (!status)
			cout <<"Couldn't set sample parameter"<< endl;
	} catch (CL_Error err) { }

	try {
		int freq = CL_Integer(resname + "freq", sndmanager);
		bool status = sb.set_frequency(freq);
		if (!status)
			cout <<"Couldn't set sample parameter"<< endl;
	} catch (CL_Error err) { }
}*/

void Sound::initPolyphony(string resname, string basename) {
/*	bool poly = false;
	try {
		poly = (bool)CL_Integer(
			resname + "polyphony", sndmanager);
	} catch (CL_Error err) { }

	polyphony.insert(pair<string, bool>(basename, poly));*/
}

Sound::~Sound() {
	stopMusic();

#ifdef HAVEMIKMOD
	for(int i=0; i < Sound::MUSEND; i++) {
		delete mus[i];
	}
#endif

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

/*	try {
		CL_SetupSound::init();*/
		/* n.b. mikmod doesn't like it if I init it, deinit
		 * it and reinit it again. It only works once.
		 */
#ifdef HAVEMIKMOD
		CL_SetupMikMod::init();
#endif

		soundobj = new Sound();
/*	} catch (CL_Error err) {
		cout << "Initialising audio failed: " << err.message << endl;
		sound = music = false;
		deinitAudio();
		throw err;
	}*/
}

void Sound::deinitAudio() {
	if (soundobj) {
		delete soundobj;
		soundobj = NULL;

#ifdef HAVEMIKMOD
		CL_SetupMikMod::deinit();
#endif

//		CL_SetupSound::deinit();
	}
}

void Sound::playSound(string resname, float pan) {
	if (sound) {
		// Figure out which variant of the sound effect to play
/*		int variants = soundobj->sfx.count(resname);
		int number = (int)(variants * ((float)rand() / RAND_MAX));
		if (!variants) cout << "Error: Sample " << 
			resname << " not found." << endl;

		// Find where the samples are
		multimap<string, CL_SoundBuffer>::iterator iter = 
			soundobj->sfx.find(resname);

		// Find the one we need
		// only a bidirectional iterator, loop instead of iter+=number
		for (; number > 0; number--) iter++;
		CL_SoundBuffer sb = (*iter).second;

		// If it's already playing, do we need to play it again?
		if (!soundobj->canPlaySound(resname)) {
			//cout << "Sample already playing" << endl;
			return;
		}

		// Play the sample
		try {
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
/*	bool playmany = (*soundobj->polyphony.find(resname)).second;
	if (playmany)	return true; // polyphony's on, play another instance

	bool isplaying = (*soundobj->playing.find(resname))
			.second.is_playing();
	return !isplaying;*/
	return false;
}

void Sound::playMusic(musindex number) {
#ifdef HAVEMIKMOD
	if (music) {
		try {
			soundobj->stopMusic();
			//mus[number]->set_position(0);
			soundobj->mus[number]->play();
		} catch(CL_Error err) {
			cout << err.message.c_str() << endl;
		}
	}
#endif
}

void Sound::stopMusic() {
#ifdef HAVEMIKMOD
	if (soundobj) {
		for (int i=0; i < MUSEND; i++) {
			soundobj->mus[i]->stop();
		}
	}
#endif
}
