// **********************************************************************
//                            Mojotron
//                        ---------------------
//
//  File        : SDL_CL_ResourceManager.h
//  Description : 
//  Author      : Dennis Payne <dulsi@identicalsoftware.com>
//  License     : GNU General Public License 2 or higher
//
// **********************************************************************

#ifndef _SDL_CL_RESOURCEMANAGER_H_
#define _SDL_CL_RESOURCEMANAGER_H_

#include <SDL.h>
#include "SDL_CL_Surface.h"
#include <unordered_map>

#define CL_ResourceManager SDL_CL_ResourceManager

class SDL_CL_ResourceManager
{
	friend class ResourceXMLParser;
	friend class SDL_CL_Surface;
	friend class SDL_CL_Font;
public:
	/** Constructor	*/
	SDL_CL_ResourceManager(const std::string &filename);

	/** Destructor */
	~SDL_CL_ResourceManager();

	bool get_boolean_resource(const std::string &name, bool d);

	int get_integer_resource(const std::string &name, int d);

	std::string get_string_resource(const std::string &name, std::string d);

	bool resource_exists(const std::string &name);

protected:
	std::unordered_map<std::string, int> _int_map;
	std::unordered_map<std::string, std::string> _string_map;
	std::unordered_map<std::string, std::shared_ptr<SDL_CL_Font_Impl> > _font_map;
	std::unordered_map<std::string, std::shared_ptr<SDL_CL_Surface_Impl> > _image_map;
//	std::unordered_map<std::string, std::shared_ptr<SDL_CL_Sprite_Impl> > _sprite_map;
};

#endif
