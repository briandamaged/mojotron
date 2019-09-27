// **********************************************************************
//                            OpenAlchemist
//                        ---------------------
//
//  File        : SDL_CL_ResourceManager.cpp
//  Description : 
//  Author      : Dennis Payne <dulsi@identicalsoftware.com>
//  License     : GNU General Public License 2 or higher
//
// **********************************************************************

#include "SDL_CL_ResourceManager.h"
#include <vector>
#include "expatcpp.h"
//#include "CommonResources.h"

#include <iostream>

class ResourceXMLParser : public ExpatXMLParser
{
public:
	ResourceXMLParser(SDL_CL_ResourceManager &rm);

	virtual void startElement(const XML_Char *name, const XML_Char **atts);
	virtual void endElement(const XML_Char *name);
	virtual void characterData(const XML_Char *s, int len);
	
	std::vector<std::string> section;
	SDL_CL_ResourceManager &_rm;
//	std::shared_ptr<SDL_CL_Sprite_Impl> _cur_sprite;
	std::string _filename;
	int _x, _y, _w, _h;
//	std::shared_ptr<SDL_CL_Font_Impl> _cur_font;
};

ResourceXMLParser::ResourceXMLParser(SDL_CL_ResourceManager &rm) : _rm(rm)//, _cur_sprite(NULL)
{
}

void ResourceXMLParser::startElement(const XML_Char *name, const XML_Char **atts)
{
	if (strcmp(name, "section") == 0)
	{
		for (int i = 0; atts[i]; i += 2)
		{
			if (strcmp(atts[i], "name") == 0)
			{
				section.push_back(atts[i + 1]);
				break;
			}
		}
	}
	else if (strcmp(name, "integer") == 0)
	{
		std::string pathname;
		int value = 0;
		for (auto itr : section)
		{
			pathname += itr;
			pathname += "/";
		}
		for (int i = 0; atts[i]; i += 2)
		{
			if (strcmp(atts[i], "name") == 0)
			{
				pathname += atts[i + 1];
			}
			if (strcmp(atts[i], "value") == 0)
			{
				value = atoi(atts[i + 1]);
			}
		}
		_rm._int_map[pathname] = value;
	}
	else if (strcmp(name, "boolean") == 0)
	{
		std::string pathname;
		int value = 0;
		for (auto itr : section)
		{
			pathname += itr;
			pathname += "/";
		}
		for (int i = 0; atts[i]; i += 2)
		{
			if (strcmp(atts[i], "name") == 0)
			{
				pathname += atts[i + 1];
			}
			if (strcmp(atts[i], "value") == 0)
			{
				if (strcmp(atts[i + 1], "true") == 0)
					value = 1;
				else
					value = 0;
			}
		}
		_rm._int_map[pathname] = value;
	}
	else if (strcmp(name, "string") == 0)
	{
		std::string pathname;
		std::string value;
		for (auto itr : section)
		{
			pathname += itr;
			pathname += "/";
		}
		for (int i = 0; atts[i]; i += 2)
		{
			if (strcmp(atts[i], "name") == 0)
			{
				pathname += atts[i + 1];
			}
			if (strcmp(atts[i], "value") == 0)
			{
				value = atts[i + 1];
			}
		}
		_rm._string_map[pathname] = value;
	}
	else if (strcmp(name, "surface") == 0)
	{
		std::shared_ptr<SDL_CL_Surface_Impl> surf = std::shared_ptr<SDL_CL_Surface_Impl>(new SDL_CL_Surface_Impl);
		std::string pathname;
		for (auto itr : section)
		{
			pathname += itr;
			pathname += "/";
		}
		for (int i = 0; atts[i]; i += 2)
		{
			if (strcmp(atts[i], "name") == 0)
			{
				pathname += atts[i + 1];
			}
			if (strcmp(atts[i], "value") == 0)
			{
				surf->_filename = atts[i + 1];
			}
			if (strcmp(atts[i], "x") == 0)
			{
				surf->_x = atol(atts[i + 1]);
			}
			if (strcmp(atts[i], "y") == 0)
			{
				surf->_y = atol(atts[i + 1]);
			}
			if (strcmp(atts[i], "width") == 0)
			{
				surf->_w = atol(atts[i + 1]);
			}
			if (strcmp(atts[i], "height") == 0)
			{
				surf->_h = atol(atts[i + 1]);
			}
			if (strcmp(atts[i], "tcol") == 0)
			{
				surf->_transparent = atol(atts[i + 1]);
			}
			if (strcmp(atts[i], "array") == 0)
			{
				surf->_col = atol(atts[i + 1]);
				const char *x_str = strchr(atts[i + 1], 'x');
				if (x_str)
				{
					surf->_row = atol(x_str + 1);
				}
			}
		}
		_rm._image_map[pathname] = surf;
	}
	else if (strcmp(name, "font") == 0)
	{
		std::shared_ptr<SDL_CL_Font_Impl> cur_font = std::shared_ptr<SDL_CL_Font_Impl>(new SDL_CL_Font_Impl);
		std::string pathname;
		for (auto itr : section)
		{
			pathname += itr;
			pathname += "/";
		}
		for (int i = 0; atts[i]; i += 2)
		{
			if (strcmp(atts[i], "name") == 0)
			{
				pathname += atts[i + 1];
			}
			if (strcmp(atts[i], "value") == 0)
			{
				cur_font->_glyphs = atts[i + 1];
			}
			if (strcmp(atts[i], "spacelen") == 0)
			{
				cur_font->_spacelen = atol(atts[i + 1]);
			}
			if (strcmp(atts[i], "subtract_width") == 0)
			{
				cur_font->_subtract_width = atol(atts[i + 1]);
			}
			if (strcmp(atts[i], "letters") == 0)
			{
				cur_font->_letters = atts[i + 1];
			}
		}
		_rm._font_map[pathname] = cur_font;
	}
}

void ResourceXMLParser::endElement(const XML_Char *name)
{
	if (strcmp(name, "section") == 0)
		section.pop_back();
/*	else if (strcmp(name, "sprite") == 0)
	{
		std::string endname = section.back();
		section.pop_back();
		std::string pathname;
		for (auto itr : section)
		{
			pathname += itr;
			pathname += "/";
		}
		pathname += endname;
		_rm._sprite_map[pathname] = _cur_sprite;
		_cur_sprite = NULL;
	}
	else if (strcmp(name, "image") == 0)
	{
		if (_cur_sprite.get())
		{
			_cur_sprite->add_frame(_filename, _x, _y, _w, _h);
		}
		else
		{
			std::shared_ptr<SDL_CL_Image_Impl> img(new SDL_CL_Image_Impl);
			img->_filename = _filename;
			img->_x = _x;
			img->_y = _y;
			img->_w = _w;
			img->_h = _h;
			std::string endname = section.back();
			section.pop_back();
			std::string pathname;
			for (auto itr : section)
			{
				pathname += itr;
				pathname += "/";
			}
			pathname += endname;
			_rm._image_map[pathname] = img;
		}
	}
	else if (strcmp(name, "font") == 0)
	{
		std::string endname = section.back();
		section.pop_back();
		std::string pathname;
		for (auto itr : section)
		{
			pathname += itr;
			pathname += "/";
		}
		pathname += endname;
		_rm._font_map[pathname] = _cur_font;
		_cur_font = NULL;
	}*/
}

void ResourceXMLParser::characterData(const XML_Char *s, int len)
{
}

/************************************************************************/
/* Constructor                                                          */
/************************************************************************/
SDL_CL_ResourceManager::SDL_CL_ResourceManager(const std::string &filename)
{
	ResourceXMLParser p(*this);
	p.parse(filename.c_str());
}

/************************************************************************/
/* Destructor                                                           */
/************************************************************************/
SDL_CL_ResourceManager::~SDL_CL_ResourceManager()
{
}

bool SDL_CL_ResourceManager::get_boolean_resource(const std::string &name, bool d)
{
	auto itr = _int_map.find(name);
	if (itr != _int_map.end())
		return itr->second;
	return d;
}

int SDL_CL_ResourceManager::get_integer_resource(const std::string &name, int d)
{
	auto itr = _int_map.find(name);
	if (itr != _int_map.end())
		return itr->second;
	return d;
}

std::string SDL_CL_ResourceManager::get_string_resource(const std::string &name, std::string d)
{
	auto itr = _string_map.find(name);
	if (itr != _string_map.end())
		return itr->second;
	return d;
}

bool SDL_CL_ResourceManager::resource_exists(const std::string &name)
{
	if (_int_map.find(name) != _int_map.end())
		return true;
	if (_string_map.find(name) != _string_map.end())
		return true;
	if (_image_map.find(name) != _image_map.end())
		return true;
/*	if (_image_map.find(name) != _image_map.end())
		return true;
	if (_font_map.find(name) != _font_map.end())
		return true;*/
	return false;
}
