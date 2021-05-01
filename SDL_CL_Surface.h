// **********************************************************************
//                            Mojotron
//                        ---------------------
//
//  File        : SDL_CL_Surface.h
//  Description : 
//  Author      : Dennis Payne <dulsi@identicalsoftware.com>
//  License     : GNU General Public License 2 or higher
//
// **********************************************************************

#ifndef _SDL_CL_SURFACE_H_
#define _SDL_CL_SURFACE_H_

#include <memory>
#include <SDL.h>
#include <list>
#include <vector>
#include <string>

#define CL_Surface SDL_CL_Surface
#define CL_Font SDL_CL_Font

class SDL_CL_ResourceManager;

class SDL_CL_Surface_Impl
{
public:
	SDL_CL_Surface_Impl();
	SDL_CL_Surface_Impl(const std::string &filename, int x = -1, int y = -1, int w = -1, int h = -1);
	
	~SDL_CL_Surface_Impl();

	void load(SDL_Renderer *r);

	void put_screen(int x, int y, int spr_no = 0);

	std::string _filename;
	char *_mask;
	SDL_Texture *_t;
	SDL_Surface *_s;
	int _x,_y;
	int _h,_w;
	int _transparent;
	int _row;
	int _col;
};

class SDL_CL_Surface
{
public:
	/** Constructor	*/
	SDL_CL_Surface();
	SDL_CL_Surface(const std::string &name, SDL_CL_ResourceManager *rm);

	/** Draw	*/
	char *get_data(int spr_no);

	int get_height();

	int get_width();

	int get_num_frames();

	bool is_null() { return _impl.get() == NULL; }

	void put_screen(int x, int y, int spr_no = 0);
	void put_screen(int x, int y, int scale_x, int scale_y, int spr_no = 0);

protected:
	std::shared_ptr<SDL_CL_Surface_Impl> _impl;
};

class SDL_CL_Font_Impl
{
public:
	class SDL_ColoredFont
	{
	public:
		SDL_ColoredFont(SDL_Color c, SDL_Texture *t) : _c(c), _t(t) {}
		~SDL_ColoredFont() { SDL_DestroyTexture(_t); }

		SDL_Color _c;
		SDL_Texture *_t;
	};

	void build_width();

//	SDL_Texture *get_colored_font(SDL_Renderer *gc, SDL_Color c);

	std::string _letters;
	int _spacelen;
	int _subtract_width;
	std::string _glyphs;
	std::vector<int> _x;
	std::vector<int> _width;
	std::shared_ptr<SDL_CL_Surface_Impl> _sprite;
	std::list<std::unique_ptr<SDL_ColoredFont> > _color_texture;
};

class SDL_CL_Font
{
public:
	SDL_CL_Font();
	SDL_CL_Font(const std::string &name, SDL_CL_ResourceManager *rm);

	void print_center(int x, int y, const std::string &text);
	void print_left(int x, int y, const std::string &text);
	void print_right(int x, int y, const std::string &text);
	//void draw_text(SDL_Renderer *r, int x, int y, const std::string &text, SDL_Color c);
	int get_text_width(const std::string &text);
	int get_height();

protected:
	std::shared_ptr<SDL_CL_Font_Impl> _impl;
};

extern void SDL_fill_rect(int x1, int y1, int x2, int y2, SDL_CL_Surface *fill_surface, int focus_x, int focus_y);

#endif
