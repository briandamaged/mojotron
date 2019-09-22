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

#ifndef OBJECTLIST_HH
#define OBJECTLIST_HH

#include "Thing.hh"

class ObjectList {
public:
	ObjectList(int maxsize);
	~ObjectList();

	Thing* getNext();	// null if beyond last
	void toListStart();
	int getNumToBeKilled();	// only used on instance 'all'

	void addThing(Thing* newthing);
	void unlistThing(Thing* dead);
	void clearLevel();
	void clearDeadStuff();
	void clearData();
	
	void doCollisionEvents(ObjectList* versus, void* f(Thing*));

private:
	int length, maxsize, next;
	// actually an array... not that the compiler cares
	Thing** contents;
};

#endif
