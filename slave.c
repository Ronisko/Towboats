#include "def.h"
// WYSYLAMY DO ZLYCH TIDÓW  W MASTERZE DODAĆ WYSYALNIE 
//TIDÓW WSZYSTKICH
// na pewno trzeba pozmieniać te shorte na shorty tak, żeby mi nigdy nie rzucało żadnych warnów, bo możliwe, że przez to się jebie
int mytid;
int numberOfShips; // set
double priority; // set
int neededTowboats; // set
int ships[5]; // set
int numberOfTowboats; // set

short activeShips[5];
short permissions[5];
short availableTowboats[10];
short reservedTowboats[10];

void addTowboats(short towboats[]) {
	int i;
	for (i = 0; i < numberOfTowboats; i ++) {
		if (towboats[i] == 1) {
			availableTowboats[i] = 1;
		}
	}
}
void removeTowboats(short towboats[]) {
	int i;
	for (i = 0; i < numberOfTowboats; i ++) {
		if (towboats[i] == 1) {
			availableTowboats[i] = 0;
		}
	}
}

void e_send( int receiver, int message ) {
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&mytid, 1, 1);

	switch(message) {
		case 1: // PERMISSION
			break;
		case 2: // REQUEST
			pvm_pkdouble(&priority, 1, 1);
			break;
		case 3: // ENTRY
		case 4: // RELEASE
			pvm_pkshort(availableTowboats, numberOfTowboats, 1);
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
	short towboats[numberOfTowboats];
	if ( pvm_nrecv(-1, PERMISSION) ) {
		pvm_upkint(&j, 1, 1);
		permissions[j] = 1; 
	}
	else if ( pvm_nrecv(-1, REQUEST) ) {
		pvm_upkint(&j, 1, 1);
		pvm_upkdouble(&receivedPriority, 1, 1);
		activeShips[j] = 1;
		if (activeShips[mytid] == 1) {
			if (receivedPriority > priority) {  
				e_send(j, PERMISSION);
			}
		}
	}
	else if ( pvm_nrecv(-1, ENTRY) ) {
		pvm_upkint(&j, 1, 1);
		pvm_upkint(towboats, numberOfTowboats, 1);
		removeTowboats(towboats);
		activeShips[j] = 0;
	}
	else if ( pvm_nrecv(-1, RELEASE) ) {
		pvm_upkint(towboats, numberOfTowboats, 1);
		addTowboats(towboats);
	}
}

void emptyArray(short array[], int length) {
	int i;
	for (i = 0; i < length; i++)
		array[i] = 0;
}

short equalArrays(short permissions[], short activeShips[]) {
    int i;
    for (i = 0; i < numberOfShips; i++)
        if (permissions[i] != activeShips[i])
            return 0;

    return 1;
}

void reserveTowboats() {
	int i;
	for (i = 0; i < numberOfTowboats; i ++) {
		if (numberOf(reservedTowboats) == neededTowboats) {
			break;
		}
		if (availableTowboats[i] == 1) {
			reservedTowboats[i] = 1;
		}
	}
}

int numberOf(short array[]) {
	int i, counter = 0;
	for (i = 0; i < numberOfTowboats; i++) {
		if (array[i] == 1) {
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
	pvm_upkdouble(&priority, 1, 1);
	pvm_upkint(&neededTowboats, 1, 1);
	pvm_upkint(ships, numberOfShips, 1);
	pvm_upkint(&numberOfTowboats, 1, 1);

	/* */
	
	short ready;

	while (true) {
		//sekcja lokalna()

		e_send(0, REQUEST);
		ready = 0;
		emptyArray(permissions, numberOfShips);

		while( !ready ) {
			e_receive();

			if ( equalArrays(permissions, activeShips) ) {
				ready = 1;
				while ( true ) {
					if (numberOf(availableTowboats) > 0) {
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
