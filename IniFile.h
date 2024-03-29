// **********************************************************************
//                            Mojotron
//                        ---------------------
//
//  File        : IniFile.h
//  Description : 
//  Author      : Guillaume Delhumeau <guillaume.delhumeau@gmail.com>
//  License     : GNU General Public License 2 or higher
//
// **********************************************************************

#ifndef _INIFILE_H_
#define _INIFILE_H_

#include <list>
#include <string>
#include <memory>

/** Ini variable */
class IniElement{

public:
	enum eType { number, words, sample };

	/** Variable name */
	std::string name;

	/** Variable value */
	std::string value;

	eType etype;
};


/** INI file (to save preferences) */
class IniFile{

public:

	/** Constructor */
	IniFile();
	
	/** Destructor */
	~IniFile();

	/** Read file */
	void read(FILE* p_file);

	/** Write file */
	void write(FILE* p_file);

	/** Clear variables */
	void clear();

	/** Add a string variable */
	void add(std::string name, std::string value);

	/** Add a boolean variable */
	void add(std::string name, bool value);

	/** Add an integer variable */
	void add(std::string name, int value);

	/** Get a string variable value */
	std::string get(std::string name, std::string default_value);

	/** Get a boolean variable value */
	bool get(std::string name, bool default_value);

	/** Get an integer variable value */
	int get(std::string name, int default_value);

	std::unique_ptr<std::list<std::string> > get_resources_of_type(IniElement::eType etype);

private:
	
	/** Variables list */
	std::list<IniElement*> _list;

};

#endif
