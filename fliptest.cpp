#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <limits.h>

class MojoApp : public CL_ClanApplication {
public:
	char* get_title() {
		return "foo";
	}

	int main(int argc, char** argv) {
		try {
			CL_SetupCore::init();
			CL_SetupDisplay::init();

			CL_Display::set_videomode(400, 300, 16, false);
			CL_ResourceManager manager = CL_ResourceManager("res.scr", false);

			CL_SurfaceProvider* sp = CL_SurfaceProvider::load("Surfaces/rockman", &manager);
			CL_SurfaceProvider* spright = new CL_Canvas(sp->get_width(), sp->get_height());

			flip(sp);
			CL_Surface* sleft = CL_Surface::create(sp);
			/*sleft->put_target(0, 0, 0, spright);*/	/* no blitter to do this */

			//flip(spright);
			CL_Surface* sright = CL_Surface::create(sp);
			/*CL_SurfaceProvider* sp = new CL_Canvas(40, 40);
			colour_grey(sp);
			CL_Surface* sright = CL_Surface::create(sp);
			CL_Surface* sleft = CL_Surface::create(sp);*/

			// Loop until the user hits escape:
			while (CL_Keyboard::get_keycode(CL_KEY_ESCAPE) == false) {
				sright->put_screen(100, 150);
				sleft->put_screen(300, 150);
				CL_Display::flip_display();
				CL_System::keep_alive();
				CL_System::sleep(50);
			}
			delete sp;

			CL_SetupDisplay::deinit();
			CL_SetupCore::deinit();
			return 0;
		} catch (CL_Error err) {
			cout << err.message << endl;
			cout << "Foo!" << endl;
		}
	}

	void flip(CL_SurfaceProvider* sp) {
		sp->lock();
		unsigned char* data = (unsigned char*) sp->get_data();

		int byte_pp = sp->get_bytes_per_pixel();
		int h = sp->get_height();
		int w = sp->get_width();
		cout << w << " by " << h << " at " << byte_pp << " bytes per pixel" << endl;

		/*unsigned char *buf = new unsigned char[byte_pp];*/

		for(int j = 0; j < h; j++) {
			for(int i = 0; i < w/2; i++) {
				char temp;
				unsigned char* current = data + i;
				unsigned char* mirror = data + (w-i-1);
				/*temp = *current;
				*current = *mirror;
				*mirror = temp;*/
				*current = UCHAR_MAX/2;
				*mirror = UCHAR_MAX;
			}
			data += w*byte_pp;
		}
		sp->unlock();
	}

	void colour_grey(CL_SurfaceProvider* sp) {
		sp->lock();
		unsigned char* data = (unsigned char*) sp->get_data();

		int byte_pp = sp->get_bytes_per_pixel();
		int h = sp->get_height();
		int w = sp->get_width();

		unsigned char *buf = new unsigned char[byte_pp];

		for(int j = 0; j < h; j++) {
			for(int i = 0; i < w; i++) {
				for (int k = 0; k < byte_pp; k++)
					*(data + i*byte_pp + k) = UCHAR_MAX/2;
			}
			data += w*byte_pp;
		}
		sp->unlock();
	}

	void colour_white(CL_SurfaceProvider* sp) {
		sp->lock();
		unsigned char* data = (unsigned char*) sp->get_data();

		int byte_pp = sp->get_bytes_per_pixel();
		int h = sp->get_height();
		int w = sp->get_width();

		unsigned char *buf = new unsigned char[byte_pp];

		for(int j = 0; j < h; j++) {
			for(int i = 0; i < w; i++) {
				for (int k = 0; k < byte_pp; k++)
					*(data + i*byte_pp + k) = UCHAR_MAX;
			}
			data += w*byte_pp;
		}
		sp->unlock();
	}
} app;
