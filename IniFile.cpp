// **********************************************************************
//                            Mojotron
//                        ---------------------
//
//  File        : IniFile.cpp
//  Description : 
//  Author      : Guillaume Delhumeau <guillaume.delhumeau@gmail.com>
//  License     : GNU General Public License 2 or higher
//
// **********************************************************************

#include "IniFile.h"
#include <iostream>

/************************************************************************/
/* Constructor                                                          */
/************************************************************************/
IniFile::IniFile()
{
}

/************************************************************************/
/* Destructor                                                           */
/************************************************************************/
IniFile::~IniFile()
{
	clear();
}

/************************************************************************/
/* Write a line                                                         */
/************************************************************************/
static void write_ln(FILE* p_file, std::string string)
{
#ifdef WIN32
	string += "\r\n";
#else
	string += "\n";
#endif
	fwrite(string.c_str(), 1, string.length(), p_file);
}

/************************************************************************/
/* Read a line                                                          */
/************************************************************************/
static std::string read_ln(FILE* p_file)
{
	std::string s = "";
	char c;
	
	while ((1 == fread(&c, 1, 1, p_file)) && (c != '\n') && (c != '\r'))
	{
		s += c;
	}

	return s;
}

/************************************************************************/
/* Read                                                                 */
/************************************************************************/
void IniFile::read(FILE* p_file)
{
	clear();
	std::list<std::string> section;

	while(!feof(p_file))
	{
		// Get line
		std::string line = read_ln(p_file);

		// Parse line
		if(line.length() >1)
		{
			int separator = line.find(" = ", 0);
			if(std::string::npos != separator)
			{
				IniElement* e = new IniElement();
				int separatorType = line.find(" (type=", separator);
				int start = 0;
				while (isspace(line[start]))
					start++;
				e -> name = line.substr(start, separator);
				if (!section.empty())
				{
					std::string path;
					for (auto itr : section)
					{
						path = path + itr + "/";
					}
					e->name = path + e->name;
				}
				e -> value = line.substr(separator + 3, separatorType - separator - 3);
				if (std::string::npos != line.find("string", separatorType))
					e->isString = true;
				else
					e->isString = false;
				_list.insert(_list.end(), e);
			}
			else
			{
				separator = line.find("section ", 0);
				if (std::string::npos != separator)
				{
					int separatorEnd = line.find(" {", separator);
					std::string name = line.substr(separator + 8, separatorEnd - separator - 8);
					section.push_back(name);
				}
				else if (!section.empty())
				{
					if (std::string::npos != line.find("}", 0))
					{
						section.pop_back();
					}
				}
			}
		}

	}
}

/************************************************************************/
/* Write                                                                */
/************************************************************************/
void IniFile::write(FILE* p_file)
{
	std::list<std::string> section;
	std::list<IniElement*>::iterator it = _list.begin();
	std::string tabs;
	while(it != _list.end())
	{
		IniElement* e = (IniElement*)*it;
		std::string nm = e->name;
		std::list<std::string> curSection;
		int separator;
		while (std::string::npos != (separator = nm.find("/", 0)))
		{
			curSection.push_back(nm.substr(0, separator));
			nm = nm.substr(separator + 1, nm.length());
		}
		auto secItr = section.begin();
		auto curSecItr = curSection.begin();
		while (secItr != section.end())
		{
			if ((curSecItr != curSection.end()) && (*secItr != *curSecItr))
				break;
			secItr++;
			curSecItr++;
		}
		int popCount = 0;
		while (secItr != section.end())
		{
			tabs.pop_back();
			write_ln(p_file, tabs + "}");
			secItr++;
			popCount++;
		}
		while (popCount != 0)
		{
			section.pop_back();
			popCount--;
		}
		while (curSecItr != curSection.end())
		{
			write_ln(p_file, tabs + "section " + *curSecItr + " {");
			tabs += "\t";
			section.push_back(*curSecItr);
			curSecItr++;
		}
		std::string line = tabs + nm + " = " + e -> value;
		if (e->isString)
			line += " (type=string);";
		else
			line += " (type=integer);";
		write_ln(p_file, line);
		it++;
	}
	auto secItr = section.begin();
	while (secItr != section.end())
	{
		tabs.pop_back();
		write_ln(p_file, tabs + "}");
		secItr++;
	}
}

/************************************************************************/
/* Clear                                                                */
/************************************************************************/
void IniFile::clear()
{
	std::list<IniElement*>::iterator it = _list.begin();
	while(!_list.empty())
	{
		IniElement* e = (IniElement*) *it;
		delete e;
		it = _list.erase(it);
	}
}

/************************************************************************/
/* Add                                                                  */
/************************************************************************/
void IniFile::add(std::string name, std::string value)
{
	std::list<IniElement*>::iterator it = _list.begin();
	while(it != _list.end())
	{
		IniElement* e = (IniElement*)*it;
		if(e -> name == name)
		{
			e -> value = value;
			return;
		}
		it++;
	}

	IniElement* e = new IniElement();
	e -> name = name;
	e -> value = value;
	e->isString = true;
	_list.insert(_list.end(), e);
}

/************************************************************************/
/* Add                                                                  */
/************************************************************************/
void IniFile::add(std::string name, bool value)
{
	std::list<IniElement*>::iterator it = _list.begin();
	while(it != _list.end())
	{
		IniElement* e = (IniElement*)*it;
		if(e -> name == name)
		{
			if(value)
				e -> value = "True";
			else
				e -> value = "False";
			return;
		}
		it++;
	}

	IniElement* e = new IniElement();
	e -> name = name;

	if(value)
		e -> value = "True";
	else
		e -> value = "False";
	e->isString = true;

	_list.insert(_list.end(), e);
}

/************************************************************************/
/* Add                                                                  */
/************************************************************************/
void IniFile::add(std::string name, int value)
{
	char tmp[30];
	sprintf(tmp, "%d", value);
	std::list<IniElement*>::iterator it = _list.begin();
	while(it != _list.end())
	{
		IniElement* e = (IniElement*)*it;
		if(e -> name == name)
		{
			e->value = tmp;
		}
		it++;
	}

	IniElement* e = new IniElement();
	e -> name = name;
	e->value = tmp;
	e->isString = false;

	_list.insert(_list.end(), e);
}

/************************************************************************/
/* Get                                                                  */
/************************************************************************/
std::string IniFile::get(std::string name, std::string default_value)
{
	std::list<IniElement*>::iterator it = _list.begin();
	while(it != _list.end())
	{
		IniElement* e = (IniElement*)*it;
		if(e -> name == name)
		{
			return e -> value;
		}
		it++;
	}
	return default_value;
}

/************************************************************************/
/* Get                                                                  */
/************************************************************************/
bool IniFile::get(std::string name, bool default_value)
{
	std::list<IniElement*>::iterator it = _list.begin();
	while(it != _list.end())
	{
		IniElement* e = (IniElement*)*it;
		if(e -> name == name)
		{
			if (e->value == std::string("True"))
				return true;
			else
				return false;
		}
		it++;
	}
	return default_value;
}

/************************************************************************/
/* Get                                                                  */
/************************************************************************/
int IniFile::get(std::string name, int default_value)
{
	std::list<IniElement*>::iterator it = _list.begin();
	while(it != _list.end())
	{
		IniElement* e = (IniElement*)*it;
		if(e -> name == name)
		{
			return atol(e->value.c_str());
		}
		it++;
	}
	return default_value;
}
