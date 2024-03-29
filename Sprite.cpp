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

#include <iostream>
#include "Sprite.hh"
#include "Globals.hh"

using namespace std;
extern Globals* globals;

Sprite::Sprite(std::string resname, CL_ResourceManager* manager) : anim(resname, manager) {
	// that globals obj better be fully constructed...
	hasorientation = (bool)globals->loadInt(resname + "flip");
	timeperframe = globals->loadInt(resname + "timeperframe");

	cancolcheck = true;

	width = anim.get_width() << 8;
	height = anim.get_height() << 8;

	// this is going to get warped if the game speed is cranked up
	if (timeperframe == 0)
	{
		int animlooptime = 1000 * 60 / globals->loadInt("Constants/musicbpm");
		int frames = getFrames();
		if (frames == 0)
			frames = 1;
		timeperframe = animlooptime / frames;
	}
}

Sprite::~Sprite() {
}

unsigned int Sprite::getFrames() {
	if (hasorientation)
		return anim.get_num_frames()/2;
	else
		return anim.get_num_frames();
}

void Sprite::draw(	int x, int y, int frame, bool facingleft, 
			int xscale, int yscale) 
{
	anim.put_screen(x >> 8, y >> 8, xscale, yscale,
			getCLFrameNum(frame, facingleft));
}

void Sprite::draw(int x, int y, int frame, bool facingleft) {
	anim.put_screen(x >> 8, y >> 8,
			getCLFrameNum(frame, facingleft));
}

int Sprite::getCLFrameNum(int thingframenum, bool facingleft) {
	if (hasorientation && !facingleft)
		return thingframenum + anim.get_num_frames()/2;
	else
		return thingframenum;
	return 0;
}

int Sprite::findLowEnd(int sizea, int offset) {
	return max(0, offset);
}

int Sprite::findHighEnd(int sizea, int sizeb, int offset) {
	int highedgeb = offset + sizeb;
	return min(highedgeb, sizea);
}

bool Sprite::andcheck(	int ax, int ay, 
			unsigned int thingframea, bool afacingleft,
			Sprite* spr2, int bx, int by, 
			unsigned int thingframeb, bool bfacingleft) 
{
	if (!cancolcheck || !(spr2->cancolcheck))
		return false;

	int bwidth = spr2->width;
	int bheight = spr2->height;

	// find out which frames each sprite is on
	char *maska = anim.get_data(getCLFrameNum(thingframea, afacingleft));
	char *maskb = spr2->anim.get_data(spr2->getCLFrameNum(thingframeb, bfacingleft));

	int xoffset = bx - ax;
	int yoffset = by - ay;
	
	/* What are the dimensions of the overlap? 
	
		+----+		Set borders to one rect
		   --------
			   
		---+-+		Is the low end of the other in borders? Yes, reset
		   --------
	
		---+-+		Is the high end of the other in borders? No
		   --------
	*/
	
	/* Find the dimensions of the overlap, left edge, right edge etc. */
	int left = findLowEnd(width, xoffset);
	int right = findHighEnd(width, bwidth, xoffset);
	int top = findLowEnd(height, yoffset);	// because low coords are at the top of the screen
	int down = findHighEnd(height, bheight, yoffset);

	// all calculations so far have been in 1/256ths of pixels
	top >>= 8; down >>= 8; left >>= 8; right >>= 8;
	xoffset >>= 8;	yoffset >>= 8;
		
	if (globals->verbosity > 2) {
		cout << "Printing x data, " << left << " to " << right << endl;
		cout << "Printing y data, " << top << " to " << down << endl;
//		cout << "Transcol: " << transcol << "   Frame:" << framea << endl;
//		cout << "Transcol: " << spr2->transcol << "   Frame:" << frameb << endl;
	}

	int pixa, pixb;

	// Iterate until a touching pixel is found
	// wahh! so poignant  :)
	for(int i = top; i < down; i++) {
               	for(int j = left; j < right; j++) {
			// addr of sprite + rows down + x coord
			pixa = (*
				( maska + anim.get_width() * i + j));

			// sprite addr + rows down + x coord - offset relative to thinga  
			pixb = (*( 
				maskb + spr2->anim.get_width() * (i - yoffset)
					+ j - xoffset
				));
		
			if (pixa!=0 && pixb!=0) {
				return true;
			}
				
			/*	Hooray for ASCII-art debugging!!
			if (pixa == transcol) {
				if (pixb == spr2->transcol) {
					cout << " ";
				} else {
					cout << "|";
				}
			} else {
				if (pixb == spr2->transcol) {
					cout << "-";
				} else {
					cout << "#";
				}
			}
			*/
		}
		//cout << endl;	
	}
	return false;
}

