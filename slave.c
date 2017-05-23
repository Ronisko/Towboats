#include "def.h"

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

int getIndexByTid(int tid) {
	int i;
	for ( i=0; i<numberOfShips; i++ ) {
		if (ships[i] == tid) {
			return i;
		}
	}
}

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
	int rcvTid;
	double receivedPriority;
	short towboats[numberOfTowboats];
	if ( pvm_nrecv(-1, PERMISSION) ) {
		pvm_upkint(&rcvTid, 1, 1);
		permissions[rcvTid] = 1; 
        }	
	if ( pvm_nrecv(-1, REQUEST) ) {
		pvm_upkint(&rcvTid, 1, 1);
		pvm_upkdouble(&receivedPriority, 1, 1);
		int index = getIndexByTid(rcvTid);
		activeShips[index] = 1;
		if (receivedPriority > priority && activeShips[mytid] == 1) {
				pvm_initsend(PvmDataDefault);
				pvm_pkint(&mytid, 1, 1);
				e_send(rcvTid, PERMISSION);
		}
	}
	if ( pvm_nrecv(-1, ENTRY) ) {
		pvm_upkint(&rcvTid, 1, 1);
		pvm_upkshort(towboats, numberOfTowboats, 1);
		removeTowboats(towboats);
		activeShips[rcvTid] = 0;
	}
	if ( pvm_nrecv(-1, RELEASE) ) {
		pvm_upkint(&rcvTid, 1, 1);
		pvm_upkshort(towboats, numberOfTowboats, 1);
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
	int i;
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
	
	for (i=0; i < numberOfShips; i++) {
		activeShips[i] = 1;
	}
	for (i=0; i < numberOfTowboats; i++) {
		availableTowboats[i] = 0;
		reservedTowboats[i] = 0;
	}
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

				removeTowboats(reservedTowboats);
				e_send(0, ENTRY);
			}
		}
		// sekcja krytyczna
		addTowboats(reservedTowboats);
		e_send(0, RELEASE);
		emptyArray(reservedTowboats, numberOfTowboats);

	}

}
