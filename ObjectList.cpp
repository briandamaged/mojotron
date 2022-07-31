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
#include "ObjectList.hh"
#include "World.hh"
#include <iostream>

using namespace std;
extern World* worldobj;
extern Globals* globals;

ObjectList::ObjectList(int ms) {
	maxsize = ms; 
	contents = new Thing*[maxsize];
	clearData();
}

ObjectList::~ObjectList() {
	// doesn't delete underlying objects
	delete[] contents;
}

void ObjectList::clearData() {
	length = 0;
	toListStart();
}

Thing* ObjectList::getNext() {
	if ((next+1) < length) {
		return contents[++next];
	}
	return 0;
}

void ObjectList::toListStart() {
	next = -1;
}

int ObjectList::getNumToBeKilled() {
	int needskill = 0;
	for(int i=0; i < length; i++) {
		if (!contents[i]->optionalkill)
			needskill++;
	}
	return needskill;
}

void ObjectList::addThing(Thing* newthing) {
	if (globals->verbosity > 1)
		cout << "Transferring new object to slot " << length << endl;

	if (length < maxsize) {
		contents[length] = newthing;
		length++;
	} else {
		// do something to clean up here
		cout << "No more objects" << endl;
		exit(EXIT_FAILURE);
	}
}

// N.B  Does not delete object, just takes it off the list
void ObjectList::unlistThing(Thing* deadthing) {
	for(int i=0; i < length; i++) {
		if (contents[i] == deadthing) {
			length--;
			contents[i] = contents[length];
			return;
		}
	}
	cout << "Couldn't find thing " << deadthing << " to remove!!" << endl;
}

// thanks Jim. (From Swarm)
void ObjectList::clearDeadStuff() {
	for(int j = 0; j < length; j++) {
		while ((j < length) && (contents[j]->health <= 0)) {
			if (globals->verbosity > 1)
				cout << "Removing #" << j << " of " << length << " objs. (health is " << contents[j]->health << ")" << endl;

			// aargh! coupling!
			if (!(contents[j]->optionalkill))
				worldobj->num_active--;

			delete contents[j];
			length--;
			contents[j] = contents[length];
		}
	}
}

void ObjectList::clearLevel() { // clears everything not marked persistant
	int j=0;
	for(int i=0; i < length; i++) {
		if (!contents[i]->levchangepersistant) {
			delete contents[i];
			contents[i] = 0;
		} else {
			contents[j] = contents[i];
			if (j < i)
				contents[i] = 0;
			j++;
		}
	}
	length = j;
}

// Not yet in use
void ObjectList::doCollisionEvents(ObjectList* versus, void* f(Thing* t)) {
	Thing* currentus, *currentversus;

	while ((currentus = getNext())) {
		while ((currentversus = versus->getNext())) {
			// check for things listening to themselves
			if (currentversus != currentus) {

				if (currentversus->colcheck(currentus)) {
					/*currentversus->*/f(currentus);
				}
			} 
		}
		versus->toListStart();
	}
}
