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

#include <stdio.h>
#include "ScoreBoard.hh"
#include "World.hh"
#include "Sprite.hh"
#include "Player.hh"

using namespace std;
extern World* worldobj;
extern Globals* globals;

ScoreBoard::ScoreBoard(Player* _p) {
	p = _p;
	highlightfruitflashing = false;
	highlightinventorytimer = highlightfruittimer = 0;
	newfruittimer = 0;
	fruit[0] = fruit[1] = fruit[2] = fruit[3] = Fruit::NONE;
}

ScoreBoard::~ScoreBoard() {

}

void ScoreBoard::drawReels(int xsofar, int yorigin) {
	CL_Surface reelsurf = globals->spr[Globals::REEL]->anim;

	// don't update the display if they just got a combo
	if (!highlightfruittimer) {
		if (!(fruit[0] == p->taken[0] && fruit[1] == p->taken[1] && fruit[2] == p->taken[2])) {
			fruit[3] = fruit[2];
			fruit[0] = p->taken[0]; fruit[1] = p->taken[1]; fruit[2] = p->taken[2];
			newfruittimer = (reelsurf.get_height())<<8;
		}
	}
	if (newfruittimer > 0)	newfruittimer -= worldobj->delta*12;
	else			newfruittimer = 0;
	
	int yfruitpos = (4 + yorigin)<<8;

	int width = 3*reelsurf.get_width() + 4;
	int height = reelsurf.get_height();

	// Window over the fruit display
	SDL_Rect old, r;
	bool clipEnabled = SDL_RenderIsClipEnabled(game_renderer);
	if (clipEnabled)
		SDL_RenderGetClipRect(game_renderer, &old);
	r.x = xsofar;
	r.y = 4 + yorigin;
	r.w = width;
	r.h = height;
	SDL_RenderSetClipRect(game_renderer, &r);
	for (int i=0; i < 3; i++) {
		reelsurf.put_screen(xsofar, yorigin);
		if (!highlightfruitflashing) {
			globals->spr[Globals::BONUSFRUIT]->draw((xsofar + 2)<<8, yfruitpos-newfruittimer, fruit[i], false);
			if (newfruittimer)
				globals->spr[Globals::BONUSFRUIT]->draw(
					(xsofar + 2)<<8, 
					(reelsurf.get_height()<<8)+yfruitpos-newfruittimer, fruit[i+1], false);
		}
		xsofar += reelsurf.get_width() + 2;
	}
	// prepare for drawing outside the window
	if (clipEnabled)
		SDL_RenderSetClipRect(game_renderer, &old);
	else
		SDL_RenderSetClipRect(game_renderer, NULL);
}

void ScoreBoard::drawScoreBoard(int xorigin, int yorigin) {
	if (p->playing) {
		if (highlightinventorytimer) {
			highlightinventorytimer -= worldobj->delta;
			if (highlightinventorytimer < 0) highlightinventorytimer = 0;
		}
		float bg = (float)highlightinventorytimer / 2000;
		SDL_Rect r;
		SDL_SetRenderDrawColor(game_renderer, bg * 255, bg * 255, bg * 255, 255);
		r.x = xorigin + 20;
		r.y = yorigin + 3;
		r.w = 60;
		r.h = BOARDHEIGHT + 1 - 3;
		SDL_RenderFillRect(game_renderer, &r);

		p->spr->draw((xorigin + PAD) << 8, (yorigin) << 8, 0, false);
		p->arm->draw((xorigin + PAD) << 8, (yorigin) << 8, 3, false);

		// shuffle over and start drawing powerups
		int xsofar = xorigin + 40;

		for (int i=3; i>0; i--) {
			if (p->inventory[i]) 
				p->inventory[i]->spr->draw((xsofar + 8*i)<<8, 
							(yorigin + 5)<<8, (unsigned int)0, false); 
		}

		if (p->inventory[0]) {
			p->inventory[0]->spr->draw(	(xsofar)<<8, 
							(yorigin + 5)<<8, (unsigned int)0, false); 
			if (p->inventory[0]->switchedon) {
				int length = p->inventory[0]->health >> 8;
				SDL_SetRenderDrawColor(game_renderer, 0, 0, 0, 255);
				r.x = xsofar-2;
				r.y = yorigin+BOARDHEIGHT - 2*PAD - 7;
				r.w = length + 2;
				r.h = 9;
				SDL_RenderFillRect(game_renderer, &r);
				SDL_SetRenderDrawColor(game_renderer, 255, 255, 255, 255);
				r.x = xsofar;
				r.y = yorigin+BOARDHEIGHT - 2*PAD - 5;
				r.w = length;
				r.h = 5;
				SDL_RenderFillRect(game_renderer, &r);
			}
		}	
		if (highlightfruittimer) {
			highlightfruittimer -= worldobj->delta;
			if (highlightfruittimer < 0) highlightfruittimer = 0;
			highlightfruitflashing = !highlightfruitflashing;
		} else {
			highlightfruitflashing = false;
		}
		drawReels(xsofar + 50, yorigin + 2);
		
	} else {
		globals->smallfont->print_left(	xorigin + 2*PAD, yorigin + PAD, "Press 2UP use key to start!");
	}
}

void ScoreBoard::highlightFruit() {
	CL_Surface reelsurf = globals->spr[Globals::REEL]->anim;
	highlightfruittimer = 2000;
	fruit[0] = p->taken[0]; fruit[1] = p->taken[1]; fruit[2] = p->taken[2];
	newfruittimer = (reelsurf.get_height())<<8;
}

void ScoreBoard::highlightInventory() {
	highlightinventorytimer = 2000;
}
