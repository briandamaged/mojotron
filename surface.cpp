/*
	Loads a single surface from a pcx file without using resource files.
*/

#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>

class SurfaceApp : public CL_ClanApplication
{
public:
	virtual char *get_title() { return "Surface application"; }

	virtual int main(int, char **)
	{
		// Create a console window for text-output if not available
		CL_ConsoleWindow console("Console");
		console.redirect_stdio();

		try
		{
			CL_SetupCore::init();
			CL_SetupDisplay::init();

			// Set mode: 320x200 16 bpp
			CL_Display::set_videomode(320, 200, 16, false);

			// Load surface from pcx file:
			CL_ResourceManager* rm = new CL_ResourceManager("res.scr", false);
			CL_SurfaceProvider *sp = CL_SurfaceProvider::load("Surfaces/livesicon", rm);
			sp->flip_vertical();
			CL_Surface *surface = CL_Surface::create(sp);

			unsigned int time = CL_System::get_time();
			int num_frames = 0;

			// Loop until the user hits escape:
			while (CL_Keyboard::get_keycode(CL_KEY_ESCAPE) == false)
			{
				// Draw surface:
				surface->put_screen(0, 0);
				
				// Flip front and backbuffer. This makes the changes visible:
				CL_Display::flip_display();

				//improve response
				CL_System::sleep(10);
				
				// Update keyboard input and handle system events:
				// Exits the loop if ClanLib requests shutdown - for instance if
				// someone closes the window.
				CL_System::keep_alive();

				num_frames++;
			}

			float fps = num_frames / ((CL_System::get_time() - time)/1000.0f);
			std::cout << "FPS: " << fps << std::endl;

			delete surface;

			CL_SetupDisplay::deinit();
			CL_SetupCore::deinit();
		}
		catch (CL_Error err)
		{
			std::cout << "Error: " << err.message.c_str() << std::endl;
		}

		// Display console close message and wait for a key
		console.display_close_message();

		return 0;
	}
} app;
