// **********************************************************************
//                            Mojotron
//                        ---------------------
//
//  File        : Ratings.h
//  Description : 
//  Author      : Dennis Payne <dulsi@identicalsoftware.com>
//  License     : GNU General Public License 2 or higher
//
// **********************************************************************

#ifndef _RATINGS_H_
#define _RATINGS_H_

#include <vector>
#include <string>
#include "SDL_CL_ResourceManager.h"

class Ratings{

public:

	/** Constructor */
	Ratings(CL_ResourceManager* manager);

	/** Read file */
	std::string rating(int score);

private:
	std::string getResName(int num, const std::string &name);

private:
	std::vector<int> _maxScore;
	std::vector<std::string> _text;
	std::string _defaultText;
};

#endif
