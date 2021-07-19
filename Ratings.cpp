// **********************************************************************
//                            Mojotron
//                        ---------------------
//
//  File        : Ratings.cpp
//  Description : 
//  Author      : Dennis Payne <dulsi@identicalsoftware.com>
//  License     : GNU General Public License 2 or higher
//
// **********************************************************************

#include "Ratings.h"

/************************************************************************/
/* Constructor                                                          */
/************************************************************************/
Ratings::Ratings(CL_ResourceManager* manager)
{
	_defaultText = manager->get_string_resource("Ratings/default", "");
	int i = 1;
	
	while (manager->resource_exists(getResName(i, "text")))
	{
		_text.push_back(manager->get_string_resource(getResName(i, "text"), ""));
		_maxScore.push_back(manager->get_integer_resource(getResName(i, "score"), 0));
		// Check to make sure score goes up
		i++;
	}
}

/************************************************************************/
/* Get the rating for a score                                           */
/************************************************************************/
std::string Ratings::rating(int score)
{
	int minimum = 0;
	for (unsigned int i = 0; i < _maxScore.size(); i++)
	{
		if ((score >= minimum) && (score <= _maxScore[i]))
		{
			return _text[i];
		}
		minimum = _maxScore[i] + 1;
	}
	return _defaultText;
}

/************************************************************************/
/* Get the resource name                                                */
/************************************************************************/
std::string Ratings::getResName(int num, const std::string &name)
{
	char number[100];
	sprintf(number, "%d", num);
	return std::string("Ratings/rating") + number + "/" + name;
}
