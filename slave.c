#include "def.h"

int numberOfShips; // set
long long priority; // set
int neededTowboats; // set
int ships[5]; // set
int numberOfTowboats; // set
int mytid;

bool activeShips[5];
bool permissions[5];
bool availableTowboats[10];
bool reservedTowboats[10];

void addTowboats(bool towboats[]) {
	int i;
	for (i = 0; i < numberOfTowboats; i ++) {
		if (towboats[i] == true) {
			availableTowboats[i] = true;
		}
	}
}
void removeTowboats(bool towboats[]) {
	int i;
	for (i = 0; i < numberOfTowboats; i ++) {
		if (towboats[i] == true) {
			availableTowboats[i] = false;
		}
	}
}

void e_send( int receiver, int message ) {
	pvm_initsend(PvmDataDefault);
	pvm_pkint(mytid, 1, 1);

	switch(message) {
		case 1: // PERMISSION
			break;
		case 2: // REQUEST
			pvm_pkint(priority, 1, 1);
			break;
		case 3: // ENTRY
		case 4: // RELEASE
			pvm_pkint(availableTowboats, numberOfTowboats, 1);
			break;
	}

	if (!receiver) {
		int i;
		for (i=0; i < numberOfShips; i ++) {
			if (ships[i] != mytid) {
				pvm_send(ships[i], message);
			}
		}
	} else {
		pvm_send(receiver, message);
	}
}

void e_receive() {
	int j, receivedPriority;
	bool towboats[numberOfTowboats];
	if ( pvm_nrecv(-1, PERMISSION) ) {
		pvm_upkint(&j, 1, 1);
		permissions[j] = true; 
	}
	else if ( pvm_nrecv(-1, REQUEST) ) {
		pvm_upkint(&j, 1, 1);
		pvm_upkint(&receivedPriority, 1, 1);
		activeShips[j] = true;
		if (activeShips[mytid] == true) {
			if (receivedPriority > priority) {  
				e_send(j, PERMISSION);
			}
		}
	}
	else if ( pvm_nrecv(-1, ENTRY) ) {
		pvm_upkint(&j, 1, 1);
		pvm_upkint(towboats, numberOfTowboats, 1);
		removeTowboats(towboats);
		activeShips[j] = false;
	}
	else if ( pvm_nrecv(-1, RELEASE) ) {
		pvm_upkint(towboats, numberOfTowboats, 1);
		addTowboats(towboats);
	}
}

void emptyArray(bool array[], int length) {
	int i;
	for (i = 0; i < length; i++)
		array[i] = false;
}

bool equalArrays(bool permissions[], bool activeShips[]) {
    int i;
    for (i = 0; i < numberOfShips; i++)
        if (permissions[i] != activeShips[i])
            return false;

    return true;
}

void reserveTowboats() {
	int i;
	for (i = 0; i < numberOfTowboats; i ++) {
		if (numberOf(reservedTowboats) == neededTowboats) {
			break;
		}
		if (availableTowboats[i] == true) {
			reservedTowboats[i] = true;
		}
	}
}

int numberOf(bool array[]) {
	int i, counter = 0;
	for (i = 0; i < numberOfTowboats; i++) {
		if (array[i] == true) {
			counter++;
		}
	}
	return counter;
}



main()
{
	mytid = pvm_mytid();

	/**
	  * internal
	  */
	pvm_recv( -1, MSG_MSTR );
	pvm_upkint(&numberOfShips, 1, 1);
	pvm_upklong(&priority, 1, 1);
	pvm_upkint(&neededTowboats, 1, 1);
	pvm_upkint(&ships, numberOfShips, 1);
	pvm_upkint(&numberOfTowboats, 1, 1);

	/* */
	
	bool ready;

	while (true) {
		//sekcja lokalna()

		e_send(0, REQUEST);
		ready = false;
		emptyArray(permissions, numberOfShips);

		while( !ready ) {
			e_receive();

			if ( equalArrays(permissions, activeShips) ) {
				ready = true;
				while ( true ) {
					if (numberOf(availableTowboats > 0)) {
						reserveTowboats();
					}
					if ( neededTowboats != numberOf( reservedTowboats ) ) {
						break; 
					}
					e_receive();
				}

				remove(reservedTowboats);
				e_send(0, ENTRY);
			} 
		}
		// sekcja krytyczna
		addTowboats(reservedTowboats);
		e_send(0, RELEASE);
		emptyArray(reservedTowboats, numberOfTowboats);

	}

}
