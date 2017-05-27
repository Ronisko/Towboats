#include "def.h"

int myIndex, masterTid, mytid, i;
int numberOfShips, neededTowboats, numberOfTowboats;
double priority;  
int *ships;

short *activeShips, *permissions;
short *availableTowboats, *reservedTowboats;
int *waitingForRelease;


void send1(int type) {
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&type, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
	pvm_send(masterTid, RAPORT);
}

void sendPerm(int ind){
	pvm_initsend(PvmDataDefault);
	int a  = 5;
	pvm_pkint(&a, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
	pvm_pkint(&ind, 1, 1);
	pvm_send(masterTid, RAPORT);
}

void sendReq(int ind, double prio, double myprio, short myActive){
	pvm_initsend(PvmDataDefault);
	int a  = 3;
	pvm_pkint(&a, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
	pvm_pkint(&ind, 1, 1);
	pvm_pkdouble(&prio, 1, 1);
	pvm_pkdouble(&myprio, 1, 1);
	pvm_pkshort(&myActive, 1, 1);
	pvm_send(masterTid, RAPORT);
}

void sendEntr(int ind, short tow[]){
	pvm_initsend(PvmDataDefault);
	int a  = 7;
	pvm_pkint(&a, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
	pvm_pkint(&ind, 1, 1);
	pvm_pkshort(tow, numberOfTowboats, 1);
	pvm_send(masterTid, RAPORT);
}

void sendRel(int ind, short tow[]){
	pvm_initsend(PvmDataDefault);
	int a  = 10;
	pvm_pkint(&a, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
	pvm_pkint(&ind, 1, 1);
	pvm_pkshort(tow, numberOfTowboats, 1);
	pvm_send(masterTid, RAPORT);
}

void sendStan() {
	pvm_initsend(PvmDataDefault);
	int a  = 11;
	pvm_pkint(&a, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
	pvm_pkshort(permissions, numberOfShips, 1);
	pvm_pkshort(activeShips, numberOfShips, 1);
	pvm_pkdouble(&priority, 1, 1);	
	pvm_send(masterTid, RAPORT);
}

void sendTowb() {
	pvm_initsend(PvmDataDefault);
	int a  = 12;
	pvm_pkint(&a, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
	pvm_pkshort(availableTowboats, numberOfTowboats, 1);
	pvm_pkshort(reservedTowboats, numberOfTowboats, 1);	
	pvm_send(masterTid, RAPORT);
}

void isOk(int ind, short tow[]){
	bool con = false;
	short *wspoldzielone = malloc (sizeof (short) * numberOfTowboats);
	for ( i=0; i<numberOfTowboats; i++ ) {
		wspoldzielone[i] = 0;
		if (tow[i] == reservedTowboats[i] && tow[i] == 1) {
			con = true;
			wspoldzielone[i] = 1;
		}
	}
	if (con) {
		pvm_initsend(PvmDataDefault);
		int a  = 13;
		pvm_pkint(&a, 1, 1);
		pvm_pkint(&myIndex, 1, 1);
		pvm_pkint(&ind, 1, 1);
		pvm_pkshort(wspoldzielone, numberOfTowboats, 1);
		pvm_send(masterTid, RAPORT);
	}
}


int getIndexByTid(int tid) {
	for ( i=0; i<numberOfShips; i++ ) {
		if (ships[i] == tid) {
			return i;
		}
	}
}

void addTowboats(short towboats[]) {
	for (i = 0; i < numberOfTowboats; i ++) {
		if (towboats[i] == 1) {
			availableTowboats[i] = 1;
		}
	}
}

void removeTowboats(short towboats[]) {
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
		case 2: // PERMISSION
				break;
		case 3: // REQUEST
			{
				activeShips[myIndex] = 1;
				permissions[myIndex] = 1;
				pvm_pkdouble(&priority, 1, 1);
				break;
			}
		case 4: // ENTRY
			{
				activeShips[myIndex] = 0;
				permissions[myIndex] = 0;
				pvm_pkshort(reservedTowboats, numberOfTowboats, 1);
				break;
			}
		case 5: // RELEASE
			{
				pvm_pkshort(reservedTowboats, numberOfTowboats, 1);
				break;
			}
	}

	if (!receiver) {
		for (i=0; i < numberOfShips; i ++) {
			if (ships[i] != mytid) {
				pvm_send(ships[i], message);
			}
		}
	} else {
		pvm_send(receiver, message);
	}
	pvm_initsend(PvmDataDefault);

	switch(message) {
		case 2: // PERMISSION
			{
				int a = 4;
				pvm_pkint(&a, 1, 1);
				break;
			}
		case 3: // REQUEST
			{
				int a = 2;
				pvm_pkint(&a, 1, 1);
				break;
			}
		case 4: // ENTRY
			{	
				int a = 6;
				pvm_pkint(&a, 1, 1);
				break;
			}
		case 5: // RELEASE
			{
				int a = 9;
				pvm_pkint(&a, 1, 1);
				break;
			}
	}
	pvm_pkint(&myIndex, 1, 1);
	pvm_pkint(&receiver, 1, 1);
	if (message == 4) {
		pvm_pkshort(reservedTowboats, numberOfTowboats, 1);
	}
	pvm_send(masterTid, RAPORT);
}

bool e_receive(int msg) {
	bool returned = false;
	
	int rcvTid;
	double receivedPriority;
	short *towboats = malloc (sizeof (short) * numberOfTowboats);
	if ( pvm_nrecv(-1, PERMISSION) ) {
		returned = true;
		pvm_upkint(&rcvTid, 1, 1);
		int index = getIndexByTid(rcvTid);
		permissions[index] = 1;
		sendPerm(index);
	}
	if ( pvm_nrecv(-1, REQUEST) ) {
		returned = true;

		pvm_upkint(&rcvTid, 1, 1);
		pvm_upkdouble(&receivedPriority, 1, 1);
		int index = getIndexByTid(rcvTid);
		activeShips[index] = 1;
		sendReq(index, receivedPriority, priority, activeShips[myIndex]);
		if (receivedPriority < priority && activeShips[myIndex] == 1) {
			pvm_initsend(PvmDataDefault);
			pvm_pkint(&mytid, 1, 1);
			e_send(rcvTid, PERMISSION);
		} else if (receivedPriority >= priority && activeShips[myIndex] == 1){
			permissions[index] = 1; //nie jestem pewien czy to jest dobrze, na pewno nie, teraz powinno byc ok
		}
	}
	if ( pvm_nrecv(-1, ENTRY) ) {
		returned = true;

		pvm_upkint(&rcvTid, 1, 1);
		pvm_upkshort(towboats, numberOfTowboats, 1);
		removeTowboats(towboats);
		int index = getIndexByTid(rcvTid);
		for (i = 0; i < numberOfTowboats; i++)
			if (towboats[i] == 1)
				waitingForRelease[i] = index;

		isOk(index, towboats);
		activeShips[index] = 0;
		permissions[index] = 0;
		sendEntr(index, towboats);
	}
	if ( pvm_nrecv(-1, RELEASE) ) {
		returned = true;
		pvm_upkint(&rcvTid, 1, 1);
		int index = getIndexByTid(rcvTid);
		pvm_upkshort(towboats, numberOfTowboats, 1);
		for (i = 0; i < numberOfTowboats; i++) {
			if (towboats[i] == 1 && waitingForRelease[i] == index) {
				availableTowboats[i] = 1;
				waitingForRelease[i] = -1;
			}
		}
		sendRel(index, towboats);
	}
	return returned;
}

void emptyArray(short array[], int length) {
	for (i = 0; i < length; i++)
		array[i] = 0;
}

short equalArrays(short permissions[], short activeShips[]) {
    for (i = 0; i < numberOfShips; i++)
        if (permissions[i] != activeShips[i])
            return 0;

    return 1;
}

void reserveTowboats() {
	int i;
	for (i = 0; i < numberOfTowboats; i++) {
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
	pvm_upkint(&masterTid, 1, 1);
	pvm_upkint(&numberOfShips, 1, 1);
	pvm_upkdouble(&priority, 1, 1);
	pvm_upkint(&neededTowboats, 1, 1);
	ships = malloc (sizeof (int) * numberOfShips);
	pvm_upkint(ships, numberOfShips, 1);
	pvm_upkint(&numberOfTowboats, 1, 1);

	activeShips = malloc (sizeof (int) * numberOfShips);
	permissions = malloc (sizeof (int) * numberOfShips);
	waitingForRelease = malloc (sizeof (int) * numberOfTowboats);
	availableTowboats = malloc (sizeof (short) * numberOfTowboats);
	reservedTowboats = malloc (sizeof (short) * numberOfTowboats);

	/* */
	
	short ready;
	myIndex = getIndexByTid(mytid);
	// for (i=0; i < numberOfShips; i++) {
		// activeShips[i] = 1;
	// }
	for (i=0; i < numberOfTowboats; i++) {
		waitingForRelease[i] = -1;
		availableTowboats[i] = 1;
		reservedTowboats[i] = 0;
	}
	while (true) {
		e_receive(0);
		//sekcja lokalna()
		sleep(1);
		send1(1);
		//sekcja lokalna()
		e_receive(0);
		emptyArray(permissions, numberOfShips);
		e_send(0, REQUEST);
		ready = 0;
		int licznik = 0;
		while( !ready ) {
			e_receive(0);
			licznik++;
			// if(licznik%1000==0) {
				// sleep(1);
				sendStan();
			// }	
			while(e_receive(0)) {
				
			}
			sendStan();
			while (!equalArrays(permissions, activeShips)) {
				e_receive(0);
			}
			sendStan();
			ready = 1;
			int licznik2 = 0;
			sendTowb();
			while ( true ) {
				if (numberOf(availableTowboats) > 0) {
					reserveTowboats();
					removeTowboats(reservedTowboats);
				}
				licznik2++;
				
				if ( neededTowboats == numberOf( reservedTowboats ) ) {
					break; //do while'a
				}
				while(e_receive(0)) {	
					sendTowb();
				}
			}
			removeTowboats(reservedTowboats);
			e_send(0, ENTRY);
			
		}
		e_receive(0);
		
		// sekcja krytyczna
		sleep(1);
		send1(8);
		// sekcja krytyczna
		
		e_receive(0);
		priority += numberOfShips;
		addTowboats(reservedTowboats);
		e_send(0, RELEASE);

		emptyArray(reservedTowboats, numberOfTowboats);
		e_receive(0);
	}
}

