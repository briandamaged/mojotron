// **********************************************************************
//                            Mojotron
//                        ---------------------
//
//  File        : SDL_CL_Surface.cpp
//  Description : 
//  Author      : Dennis Payne <dulsi@identicalsoftware.com>
//  License     : GNU General Public License 2 or higher
//
// **********************************************************************

#include "SDL_CL_Surface.h"
#include "SDL_CL_ResourceManager.h"
#include <SDL_image.h>
#include "Display.hh"

/************************************************************************/
/* Constructor                                                          */
/************************************************************************/
SDL_CL_Surface_Impl::SDL_CL_Surface_Impl()
	: _mask(NULL), _s(NULL), _t(NULL), _x(-1), _y(-1), _w(-1), _h(-1), _transparent(-1), _row(1), _col(1)
{
}

SDL_CL_Surface_Impl::SDL_CL_Surface_Impl(const std::string &filename, int x, int y, int w, int h)
	: _filename(filename), _mask(NULL), _s(NULL), _t(NULL), _x(x), _y(y), _w(w), _h(h), _transparent(-1), _row(1), _col(1)
{
}

/************************************************************************/
/* Destructor                                                           */
/************************************************************************/
SDL_CL_Surface_Impl::~SDL_CL_Surface_Impl()
{
    SDL_FreeSurface(_s);
	SDL_DestroyTexture(_t);
	delete [] _mask;
}

void SDL_CL_Surface_Impl::load(SDL_Renderer *r)
{
	if (_x == -1)
		_x = 0;
	if (_y == -1)
		_y = 0;
	int w, h;
	_s = IMG_Load(_filename.c_str());
	w = _s->w;
	h = _s->h;
	if (_w == -1)
		_w = w;
	if (_h == -1)
		_h = h;
	_mask = new char[_w * _h * _col * _row];
	if ((_s->format->BytesPerPixel == 1) && (_s->format->Rmask == 0) && (_s->format->Gmask == 0) && (_s->format->Bmask == 0) && (_s->format->Amask == 0))
	{
		SDL_Surface *img_convert = SDL_ConvertSurfaceFormat(_s, SDL_PIXELFORMAT_RGBA8888, 0);
		SDL_LockSurface(_s);
		SDL_LockSurface(img_convert);
		Uint32 alpha_pixel = SDL_MapRGBA(img_convert->format, 0, 0, 0, 0);
		for (int y = 0; y < _s->h; y++)
		{
			for (int x = 0; x < _s->w; x++)
			{
				if (*((unsigned char*)(_s->pixels + (y * _s->pitch) + x * _s->format->BytesPerPixel)) == _transparent)
					*((Uint32*)(img_convert->pixels + (y * img_convert->pitch) + x * img_convert->format->BytesPerPixel)) = alpha_pixel;
			}
		}
		for (int r = 0; r < _row; r++)
		{
			for (int c = 0; c < _col; c++)
			{
				for (int y = 0; y < _h; y++)
				{
					for (int x = 0; x < _w; x++)
					{
						if (*((unsigned char*)(_s->pixels + (((r * _h) + y) * _s->pitch) + ((c * _w) + x) * _s->format->BytesPerPixel)) == _transparent)
							_mask[(r * _col + c) * (_w * _h) + (y * _w) + x]  = 0;
						else
							_mask[(r * _col + c) * (_w * _h) + (y * _w) + x]  = 1;
					}
				}
			}
		}
		SDL_UnlockSurface(img_convert);
		SDL_UnlockSurface(_s);
		SDL_SetSurfaceBlendMode(img_convert, SDL_BLENDMODE_BLEND);
		_t = SDL_CreateTextureFromSurface(r, img_convert);
		SDL_FreeSurface(img_convert);
	}
	else
	{
		SDL_SetSurfaceBlendMode(_s, SDL_BLENDMODE_BLEND);
		_t = SDL_CreateTextureFromSurface(r, _s);
		for (int r = 0; r < _row; r++)
		{
			for (int c = 0; c < _col; c++)
			{
				for (int y = 0; y < _h; y++)
				{
					for (int x = 0; x < _w; x++)
					{
						if (((*((Uint32*)(_s->pixels + (((r * _h) + y) * _s->pitch) + ((c * _w) + x) * _s->format->BytesPerPixel))) & _s->format->Amask) != 0)
							_mask[(r * _col + c) * (_w * _h) + (y * _w) + x]  = 1;
						else
							_mask[(r * _col + c) * (_w * _h) + (y * _w) + x]  = 0;
					}
				}
			}
		}
	}
}

void SDL_CL_Surface_Impl::put_screen(int x, int y, int spr_no /* = 0*/)
{
	int col = spr_no % _col;
	int row = spr_no / _col;
	SDL_Rect destrect;
	destrect.x = x;
	destrect.y = y;
	destrect.w = _w;
	destrect.h = _h;
	SDL_Rect srcrect;
	srcrect.x = _x + _w * col;
	srcrect.y = _y + _h * row;
	srcrect.w = _w;
	srcrect.h = _h;
	SDL_RenderCopy(game_renderer, _t, &srcrect, &destrect);
}

/************************************************************************/
/* Constructor                                                          */
/************************************************************************/
SDL_CL_Surface::SDL_CL_Surface()
{
}

SDL_CL_Surface::SDL_CL_Surface(const std::string &name, SDL_CL_ResourceManager *rm)
{
	auto itr = rm->_image_map.find(name);
	if (itr != rm->_image_map.end())
	{
		_impl = itr->second;
		_impl->load(game_renderer);
	}
}

char *SDL_CL_Surface::get_data(int spr_no)
{
	return _impl->_mask + spr_no * _impl->_w * _impl->_h;
}

int SDL_CL_Surface::get_height()
{
	if (_impl.get())
	{
		return _impl->_h;
	}
	return 0;
}

int SDL_CL_Surface::get_width()
{
	if (_impl.get())
	{
		return _impl->_w;
	}
	return 0;
}

int SDL_CL_Surface::get_num_frames()
{
	if (_impl.get())
	{
		return _impl->_row * _impl->_col;
	}
	return 0;
}

void SDL_CL_Surface::put_screen(int x, int y, int spr_no /* = 0*/)
{
	_impl->put_screen(x, y, spr_no);
}

void SDL_CL_Surface::put_screen(int x, int y, int scale_x, int scale_y, int spr_no /* = 0*/)
{
	int col = spr_no % _impl->_col;
	int row = spr_no / _impl->_col;
	SDL_Rect destrect;
	destrect.x = x;
	destrect.y = y;
	destrect.w = scale_x;
	destrect.h = scale_y;
	SDL_Rect srcrect;
	srcrect.x = _impl->_x + _impl->_w * col;
	srcrect.y = _impl->_y + _impl->_h * row;
	srcrect.w = _impl->_w;
	srcrect.h = _impl->_h;
	SDL_RenderCopy(game_renderer, _impl->_t, &srcrect, &destrect);
}

void SDL_CL_Font_Impl::build_width()
{
	int startX = 0;
	for (int indx = 0; _letters[indx] != 0; indx++)
	{
		int w = 0;
		SDL_Surface *img = _sprite->_s;
		SDL_PixelFormat *fmt = img->format;
		Uint32 alpha_pixel = SDL_MapRGBA(img->format, 0, 0, 0, 0);
		bool found = false;
		while (!found)
		{
			for (int y = 0; y < _sprite->_h; y++)
			{
				if (((*((Uint32*)(img->pixels + (y * img->pitch) + startX * fmt->BytesPerPixel))) & img->format->Amask) != 0)
				{
					found = true;
					break;
				}
			}
			if (!found)
				startX++;
		}
		found = false;
		for (int x = startX; x < _sprite->_w; x++)
		{
			found = false;
			for (int y = 0; y < _sprite->_h; y++)
			{
				if (((*((Uint32*)(img->pixels + (y * img->pitch) + x * fmt->BytesPerPixel))) & img->format->Amask) != 0)
				{
					found = true;
				}
			}
			if (!found)
			{
				w = x - startX + 1;
				break;
			}
		}
		_x.push_back(startX);
		_width.push_back(w);
		startX += w;
	}
}

/*SDL_Texture *SDL_CL_Font_Impl::get_colored_font(SDL_Renderer *gc, SDL_Color c)
{
	for (auto itr = _color_texture.begin(); itr != _color_texture.end(); itr++)
	{
		if ((c.r == (*itr)->_c.r) && (c.g == (*itr)->_c.g) && (c.b == (*itr)->_c.b))
			return (*itr)->_t;
	}
	SDL_RWops *rwopsmem = SDL_RWFromFile(_sprite->_filename.c_str(), "rb");
	SDL_Surface *img = IMG_Load_RW(rwopsmem, 1);
	if ((img->format->BytesPerPixel == 1) && (img->format->Rmask == 0) && (img->format->Gmask == 0) && (img->format->Bmask == 0) && (img->format->Amask == 0))
	{
		SDL_Surface *img_convert = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA8888, 0);
		SDL_FreeSurface(img);
		img = img_convert;
	}
	{
		SDL_PixelFormat *fmt = img->format;
		Uint32 pixel = SDL_MapRGB(img->format, c.r, c.g, c.b);
		Uint32 alpha_pixel = SDL_MapRGBA(img->format, 0, 0, 0, 0);
		for (int y = 0; y < img->h; y++)
		{
			for (int x = 0; x < img->w; x++)
			{
				if (*((Uint32*)(img->pixels + (y * img->pitch) + x * fmt->BytesPerPixel)) != alpha_pixel)
					*((Uint32*)(img->pixels + (y * img->pitch) + x * fmt->BytesPerPixel)) = pixel;
			}
		}
	}
	SDL_Texture *t = SDL_CreateTextureFromSurface(gc, img);
	SDL_FreeSurface(img);
	_color_texture.push_back(std::unique_ptr<SDL_CL_Font_Impl::SDL_ColoredFont>(new SDL_CL_Font_Impl::SDL_ColoredFont(c, t)));
	return t;
}*/

SDL_CL_Font::SDL_CL_Font()
{
}

SDL_CL_Font::SDL_CL_Font(const std::string &name, SDL_CL_ResourceManager *rm)
{
	_impl = rm->_font_map[name];
	_impl->_sprite = std::shared_ptr<SDL_CL_Surface_Impl>(new SDL_CL_Surface_Impl(_impl->_glyphs));
	_impl->_sprite->load(game_renderer);
	_impl->build_width();
}

void SDL_CL_Font::print_center(int x, int y, const std::string &text)
{
	x -= get_text_width(text) / 2;
	print_left(x, y, text);
}

void SDL_CL_Font::print_left(int x, int y, const std::string &text)
{
	int orig_x = x;
	for (int i = 0; text[i] != 0; i++)
	{
		if (text[i] == ' ')
			x += _impl->_spacelen;
		else if (text[i] == '\n')
		{
			x = orig_x;
			y += get_height();
		}
		else
		{
			for (int indx = 0; _impl->_letters[indx] != 0; indx++)
			{
				if (_impl->_letters[indx] == text[i])
				{
					SDL_Rect destrect;
					destrect.x = x;
					destrect.y = y;
					destrect.w = _impl->_width[indx];
					destrect.h = _impl->_sprite->_h;
					SDL_Rect srcrect;
					srcrect.x = _impl->_x[indx];
					srcrect.y = 0;
					srcrect.w = _impl->_width[indx];
					srcrect.h = _impl->_sprite->_h;
					SDL_RenderCopy(game_renderer, _impl->_sprite->_t, &srcrect, &destrect);
					x += _impl->_width[indx] - _impl->_subtract_width;
					break;
				}
			}
		}
	}
}

void SDL_CL_Font::print_right(int x, int y, const std::string &text)
{
	x -= get_text_width(text);
	print_left(x, y, text);
}

/*void SDL_CL_Font::draw_text(SDL_Renderer *r, int x, int y, const std::string &text, SDL_Color c)
{
	SDL_Texture *colored_tex = _impl->get_colored_font(r, c);
	y -= _impl->_sprite->frame[0]->_h;
	for (int i = 0; text[i] != 0; i++)
	{
		if (text[i] == ' ')
			x += _impl->_spacelen;
		else
		{
			for (int indx = 0; _impl->_letters[indx] != 0; indx++)
			{
				if (_impl->_letters[indx] == text[i])
				{
					std::shared_ptr<SDL_CL_Image_Impl> char_img = _impl->_sprite->frame[indx];
					SDL_Rect destrect;
					destrect.x = x;
					destrect.y = y;
					destrect.w = char_img->_w;
					destrect.h = char_img->_h;
					SDL_Rect srcrect;
					srcrect.x = char_img->_x;
					srcrect.y = char_img->_y;
					srcrect.w = char_img->_w;
					srcrect.h = char_img->_h;
					SDL_RenderCopy(r, colored_tex, &srcrect, &destrect);
					x += char_img->_w;
					break;
				}
			}
		}
	}
}*/

int SDL_CL_Font::get_text_width(const std::string &text)
{
	int width = 0;
	for (int i = 0; text[i] != 0; i++)
	{
		if (text[i] == ' ')
			width += _impl->_spacelen;
		else
		{
			for (int indx = 0; _impl->_letters[indx] != 0; indx++)
			{
				if (_impl->_letters[indx] == text[i])
				{
					width += _impl->_width[indx] - _impl->_subtract_width;
					break;
				}
			}
		}
	}
	return width;
}

int SDL_CL_Font::get_height()
{
	int height = _impl->_sprite->_h;
	return height;
}

void SDL_fill_rect(int x1, int y1, int x2, int y2, SDL_CL_Surface *fill_surface, int focus_x, int focus_y)
{
	SDL_Rect old, r;
	bool clipEnabled = SDL_RenderIsClipEnabled(game_renderer);
	if (clipEnabled)
		SDL_RenderGetClipRect(game_renderer, &old);
	r.x = x1;
	r.y = y1;
	r.w = x2 - x1;
	r.h = y2 - y1;
	SDL_RenderSetClipRect(game_renderer, &r);

	int width = fill_surface->get_width();
	int height = fill_surface->get_height();

	int x, y;
	if (focus_x < x1)
	{
		for (x=focus_x;x+width<x1;x+=width);
	}
	else
	{
		for (x=focus_x;x>x1;x-=width);
	}
	if (focus_y < y1)
	{
		for (y=focus_y;y+height<y1;y+=height);
	}
	else
	{
		for (y=focus_y;y>y1;y-=height);
	}

	for (;y<y2;)
	{
		for (int it_x=x;it_x<x2;it_x+=width)
		{
			fill_surface->put_screen(it_x, y);
		}
		y += height;
	}

	if (clipEnabled)
		SDL_RenderSetClipRect(game_renderer, &old);
	else
		SDL_RenderSetClipRect(game_renderer, NULL);
}
