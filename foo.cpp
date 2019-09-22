
/*** A hacked SoundBuffer example. On my system this segfaults on exit. ***/

#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/sound.h>

class SoundbufferApp : public CL_ClanApplication
{
public:
	virtual char *get_title() { return "Soundbuffer application"; }

	virtual int main(int, char **)
	{
		CL_SetupCore::init();	
		CL_SetupSound::init();

		CL_SoundBuffer *soundbuffer = CL_Sample::create("beep.wav", NULL);

		/*
		As the example originally was
		CL_SoundBuffer_Session *ses = new CL_SoundBuffer_Session(soundbuffer->play());
		*/

		// Prevents polyphony when play gets called lots of times
		CL_SoundBuffer_Session* ses = new CL_SoundBuffer_Session(soundbuffer->prepare());
		ses->play();

		while(ses->is_playing())
		{
			CL_System::keep_alive();
			CL_System::sleep(1000);
		}

		delete soundbuffer;

		CL_SetupSound::deinit();
		CL_SetupCore::deinit();			
		delete ses;	// this is effectively the situation when we have a non-pointer ses

		return 0;
	}
} app;
