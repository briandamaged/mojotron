
#include <ClanLib/core.h>
#include "Globals.hh"
#include "Sprite.hh"

class MojoApp : public CL_ClanApplication {
	public:

	virtual char* get_title() {
		return "Mojotron";
	}
	
	virtual int main(int argc, char** argv) {
		try {
			CL_SetupCore::init();
			CL_SetupCore::init_display();
			std::string resourcefile = "res.scr";
			CL_Display::set_videomode(640, 480, 16, false);
			Globals::loadAll(resourcefile);
		
			Globals::largefont->print_left(100, 200, "FOO");
			CL_Display::flip_display();
			Globals::largefont->print_left(100, 200, "FOO");

			int start_time = CL_System::get_time();
			int timer = start_time;

			while(timer < 10000+start_time) {
				timer = CL_System::get_time();
				CL_Display::set_translate_offset(0, timer/30);
				cout << timer/30 << endl;
				CL_System::sleep(20);
				//CL_Display::flip_display(true);
				CL_System::keep_alive();
			}
			
			Globals::deinit();
			CL_SetupCore::deinit();
			CL_SetupCore::deinit_display();

		// catch for the whole thing!!?
		} catch (CL_Error err) {
			cout << err.message.c_str() << endl;
		}
		return 0;
	}
} app;

