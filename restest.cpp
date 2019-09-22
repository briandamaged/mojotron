#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>

// g++ -lclanCore -lclanDisplay -lclanApp restest.cpp

class TestApp : public CL_ClanApplication {
	public:
	virtual char* get_title() {
		return "Poo";
	}
	
	virtual int main(int argc, char** argv) {
		CL_SetupCore::init();
		CL_SetupDisplay::init();
	
		CL_ResourceManager* manager = new CL_ResourceManager("res.scr", false);
		CL_SurfaceProvider* x = CL_SurfaceProvider::load("Testing/test", manager);
		delete x;
		delete manager;
		
		return 0;
	}
} app;

