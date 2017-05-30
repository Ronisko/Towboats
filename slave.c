#include "def.h"

int myIndex, masterTid, mytid;
int numberOfShips, neededTowboats, numberOfTowboats;
double *priorities;
int *ships;

short *activeShips;
short *availableTowboats, *reservedTowboats;
int *waitingForRelease;
bool permi;


double current_timestamp() {
	struct timeval te; 
	gettimeofday(&te, NULL);
	double milliseconds = te.tv_sec*1000 + te.tv_usec/1000;
	return milliseconds;
}

void sendStan() {
	pvm_initsend(PvmDataDefault);
	int a  = 11;
	pvm_pkint(&a, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
	pvm_pkdouble(priorities, numberOfShips, 1);
	pvm_pkshort(activeShips, numberOfShips, 1);
	pvm_pkdouble(&priorities[myIndex], 1, 1);
	pvm_send(masterTid, RAPORT);
}

void send1(int type) {
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&type, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
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

void sendPerm(int ind){
	pvm_initsend(PvmDataDefault);
	int a  = 5;
	pvm_pkint(&a, 1, 1);
	pvm_pkint(&myIndex, 1, 1);
	pvm_pkint(&ind, 1, 1);
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
	int i;
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
	int i;
	for ( i=0; i<numberOfShips; i++ )
		if (ships[i] == tid)
			return i;
}

void addTowboats(short towboats[]) {
	int i;
	for (i = 0; i < numberOfTowboats; i ++)
		if (towboats[i] == 1)
			availableTowboats[i] = 1;

}

void removeTowboats(short towboats[]) {
	int i;
	for (i = 0; i < numberOfTowboats; i ++)
		if (towboats[i] == 1)
			availableTowboats[i] = 0;
}

int nextReserving() {
	int i, index = -1;
	double min = -1;
	for(i=0; i<numberOfShips; i++){
		if(activeShips[i] == 1 && i != myIndex){
			if(priorities[i] < min || min < 0){
				min = priorities[i];
				index = i;
			}
		}
	}
	return index;
}


void e_send( int receiver, int message ) {
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&myIndex, 1, 1);
	switch(message) {
		case 3: // REQUEST
			{
				activeShips[myIndex] = 1;
				priorities[myIndex] = current_timestamp() + (myIndex*0.1);
				pvm_pkdouble(&priorities[myIndex], 1, 1);
				break;
			}
		case 4: // ENTRY
			{
				activeShips[myIndex] = 0;
				pvm_pkshort(reservedTowboats, numberOfTowboats, 1);
				int next = nextReserving();
				pvm_pkint(&next, 1, 1);
				break;
			}
		case 5: // RELEASE
			{
				pvm_pkshort(reservedTowboats, numberOfTowboats, 1);
				break;
			}
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

bool e_receive(bool blocking) {
	int index, i, next;
	bool returned = false;

	double receivedPriority;
	short *towboats = malloc (sizeof (short) * numberOfTowboats);

	if ( pvm_nrecv(-1, REQUEST) ) {
		returned = true;
		pvm_upkint(&index, 1, 1);
		pvm_upkdouble(&receivedPriority, 1, 1);
		priorities[index] = receivedPriority;
		activeShips[index] = 1;
		sendReq(index, receivedPriority, priorities[myIndex], activeShips[myIndex]);
	}

	if ( pvm_nrecv(-1, RELEASE) ) {
		returned = true;
		pvm_upkint(&index, 1, 1);
		pvm_upkshort(towboats, numberOfTowboats, 1);
		for (i = 0; i < numberOfTowboats; i++) {
			if (towboats[i] == 1 && waitingForRelease[i] == index) {
				availableTowboats[i] = 1;
				waitingForRelease[i] = -1;
			}
		}
		sendRel(index, towboats);
	}
	
	if(blocking) {
		pvm_recv(-1, ENTRY);
		returned = true;
		pvm_upkint(&index, 1, 1);
		pvm_upkshort(towboats, numberOfTowboats, 1);
		pvm_upkint(&next, 1, 1);
		removeTowboats(towboats);
		for (i = 0; i < numberOfTowboats; i++)
			if (towboats[i] == 1)
				waitingForRelease[i] = index;
			
		if (next == myIndex) {
			permi = true;
		}
		isOk(index, towboats);
		activeShips[index] = 0;
		sendEntr(index, towboats);
	} else {
		if ( pvm_nrecv(-1, ENTRY) ) {
			returned = true;
			pvm_upkint(&index, 1, 1);
			pvm_upkshort(towboats, numberOfTowboats, 1);
			pvm_upkint(&next, 1, 1);
			removeTowboats(towboats);
			for (i = 0; i < numberOfTowboats; i++)
				if (towboats[i] == 1)
					waitingForRelease[i] = index;

			if (next == myIndex) {
				permi = true;

			}
			isOk(index, towboats);
			activeShips[index] = 0;
			sendEntr(index, towboats);
		}
	}
	free(towboats);
	return returned;
}


void emptyArray(short array[], int length) {
	int i;
	for (i = 0; i < length; i++)
		array[i] = 0;
}

bool isAvailable() {
 	int i;
 	for (i = 0; i < numberOfShips; i++) {
 		if (i != myIndex && activeShips[i] != 0) {
 			if (priorities[i] <= priorities[myIndex]) {
 				return false;
 			}
 		}
 	}
	return true;
}

bool allNeededReserved() {
	if (numberOf(reservedTowboats) == neededTowboats)
		return true;

	return false;
}

void reserveTowboats() {
	int i;
	for (i = 0; i < numberOfTowboats; i++) {
		if ( allNeededReserved() ) {
			break;
		}
		if ( availableTowboats[i] == 1 ) {
			reservedTowboats[i] = 1;
		}
	}
}

int numberOf(short array[]) {
	int i, counter = 0;
	for (i = 0; i < numberOfTowboats; i++)
		if (array[i] == 1)
			counter++;

	return counter;
}
void checkMessages() {
	while(e_receive(false)) {}
}

main()
{
	int i;
	mytid = pvm_mytid();
	/**
	  * internal
	  */
	pvm_recv( -1, MSG_MSTR );
	pvm_upkint(&masterTid, 1, 1);
	pvm_upkint(&numberOfShips, 1, 1);
	priorities = malloc (sizeof (double) * numberOfShips);
	pvm_upkdouble(priorities, numberOfShips, 1);
	pvm_upkint(&neededTowboats, 1, 1);
	ships = malloc (sizeof (int) * numberOfShips);
	pvm_upkint(ships, numberOfShips, 1);
	pvm_upkint(&numberOfTowboats, 1, 1);

	activeShips = malloc (sizeof (int) * numberOfShips);
	waitingForRelease = malloc (sizeof (int) * numberOfTowboats);
	availableTowboats = malloc (sizeof (short) * numberOfTowboats);
	reservedTowboats = malloc (sizeof (short) * numberOfTowboats);
	/* */
	
	myIndex = getIndexByTid(mytid);
	if (myIndex == 0) {
		permi = true;
	} else {
		permi = false;
	}

	for (i=0; i < numberOfTowboats; i++) {
		waitingForRelease[i] = -1;
		availableTowboats[i] = 1;
		reservedTowboats[i] = 0;
	}

	while (true) {
		checkMessages();
		//sekcja lokalna()
		sleep(1);
		send1(1);
		//sekcja lokalna()
		checkMessages();
		e_send(0, REQUEST);

		checkMessages();

		sendStan();

		checkMessages();
		
		sendStan();
		
		while (!permi) {
			while(e_receive(true)) {}
			sendStan();
		}
		
		checkMessages();
		// while(!isAvailable()) {
		// 	e_receive(false);
		// }

		sendTowb();
		while ( !allNeededReserved() ) {
			if (numberOf(availableTowboats) > 0) {	
				reserveTowboats();
				removeTowboats(reservedTowboats);
				sendStan();
			
			}
			checkMessages();
		}
		sendTowb();
		removeTowboats(reservedTowboats);
		checkMessages();
		while(nextReserving() == -1 || nextReserving() == myIndex) {
			e_receive(false);
		}
		e_send(0, ENTRY);
		permi = false;
		

		checkMessages();
		
		// sekcja krytyczna
		sleep(1);
		send1(8);
		// sekcja krytyczna
		
		checkMessages();
		addTowboats(reservedTowboats);
		e_send(0, RELEASE);

		emptyArray(reservedTowboats, numberOfTowboats);
		checkMessages();
	}
}

