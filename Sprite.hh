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

#ifndef SPRITE_HH
#define SPRITE_HH

#include <ClanLib/core.h>
#include <ClanLib/display.h>

class Sprite {
private:
	int findLowEnd(int sizea, int offset);
	int findHighEnd(int sizea, int sizeb, int offset);
	int getCLFrameNum(int thingframenum, bool facingleft);

	//long* mask;
	char* mask;
	CL_SurfaceProvider* animp;
	bool cancolcheck;

public:
	//Sprite(CL_SurfaceProvider* _animp);
	Sprite(std::string resname, CL_ResourceManager* manager);
	virtual ~Sprite();

	//long* getData();
	char* getData();
	unsigned int getFrames();
	void draw(int x, int y, int frame, bool facingleft);
	void draw(int x, int y, int frame, bool facingleft, int xscale, int yscale);
	bool andcheck(	int xpos, int ypos,
			unsigned int framea, bool afacingleft,
			Sprite* other, int bx, int by,
			unsigned int frameb, bool bfacingleft);
	CL_Surface* anim;

	// colcheck info
	int width, height, pitch, transcol;
	bool hasorientation;
};

#endif
