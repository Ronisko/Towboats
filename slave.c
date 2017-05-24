#include "def.h"

int myIndex;
int masterTid;
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

FILE *fp;

void openFile() {
	fp = fopen("slave3.txt", "a+");
}
void closeFile() {
	fclose(fp);
}

void saveInt(int s) {
	openFile();
	fprintf(fp, "%d\t", mytid);
	fprintf(fp, "%d\n", s);
	closeFile();
}
void saveDouble(double d) {
	openFile();
	fprintf(fp, "%d\t", mytid);
	fprintf(fp, "%f\n", d);
	closeFile();
}
void saveChar(char c[]){
	openFile();
	fprintf(fp, "%d\t", mytid);
	fprintf(fp, "%s\n", c);
	closeFile();
}

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
	saveInt(mytid);
	saveChar("Pakuje mytid w e_send");
	switch(message) {
		case 2: // PERMISSION
			{
				break;
			}
		case 3: // REQUEST
			{
				pvm_pkdouble(&priority, 1, 1);
				saveDouble(priority);
				saveChar("Pakuje priority w e_send case 3");
				break;
			}
		case 4: // ENTRY
		case 5: // RELEASE
			{
				pvm_pkshort(availableTowboats, numberOfTowboats, 1);
				break;
			}
	}

	if (!receiver) {
		int i;
		for (i=0; i < numberOfShips; i ++) {
			if (ships[i] != mytid) {
				saveInt(ships[i]);
			        saveChar("Wysylam do statku z tid");

				pvm_send(ships[i], message);
			}
		}
	} else {
		saveInt(receiver);
		saveChar("Wysylam do statku z tid");

		pvm_send(receiver, message);
	}
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&mytid, 1, 1);

	switch(message) {
		case 2: // PERMISSION
			{
				pvm_pkstr("PERMI");
				break;
			}
		case 3: // REQUEST
			{
				pvm_pkstr("REQUE");
				break;
			}
		case 4: // ENTRY
			{
				pvm_pkstr("ENTRY");
				break;
			}
		case 5: // RELEASE
			{
				pvm_pkstr("RELEA");
				break;
			}
	}
	pvm_send(masterTid, RAPORT);
}

void e_receive() {
	int rcvTid;
	double receivedPriority;
	short towboats[numberOfTowboats];
	if ( pvm_nrecv(-1, PERMISSION) ) {
		pvm_upkint(&rcvTid, 1, 1);
		saveInt(rcvTid);
		saveChar("Odpakowuje rcvTid w e_receive PERMISSION");
		int index = getIndexByTid(rcvTid);
		permissions[index] = 1; 
        }
	if ( pvm_nrecv(-1, REQUEST) ) {
		pvm_upkint(&rcvTid, 1, 1);
		saveInt(rcvTid);
                saveChar("Odpakowuje rcvTid w e_receive REQUEST");
		pvm_upkdouble(&receivedPriority, 1, 1);
		saveDouble(receivedPriority);
                saveChar("Odpakowuje receivedPriority w e_receive REQUEST");
		int index = getIndexByTid(rcvTid);
		activeShips[index] = 1;
		if (receivedPriority < priority && activeShips[myIndex] == 1) {
				pvm_initsend(PvmDataDefault);
				pvm_pkint(&mytid, 1, 1);
				e_send(rcvTid, PERMISSION);
		}
	}
	if ( pvm_nrecv(-1, ENTRY) ) {
		saveChar("nrecv ENTRY");
		pvm_upkint(&rcvTid, 1, 1);
		pvm_upkshort(towboats, numberOfTowboats, 1);
		removeTowboats(towboats);
		int index = getIndexByTid(rcvTid);
		activeShips[index] = 0;
	}
	if ( pvm_nrecv(-1, RELEASE) ) {
		saveChar("nrecv RELEASE");
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
	myIndex = getIndexByTid(mytid);
	/**
	  * internal
	  */
	pvm_recv( -1, MSG_MSTR );
	pvm_upkint(&masterTid, 1, 1);
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
		sleep(1);
		//sekcja lokalna()
		activeShips[myIndex] = 1;
		e_send(0, REQUEST);
		ready = 0;
		emptyArray(permissions, numberOfShips);
		int hehe = 0;
		while( !ready ) {
			//saveChar("e_receive w petli");
			e_receive();
			/*if (hehe == 1) {
				while(true) {}
			}*/	
			hehe++;
			if (hehe%1000==0) {
				sleep(1);
				saveChar("Nieskonczonosc");
			}
			permissions[myIndex] = 1;
			if ( equalArrays(permissions, activeShips) ) {
				saveChar("Rownaja mi sie arraysy");
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
				activeShips[myIndex] = 0;
				removeTowboats(reservedTowboats);
				e_send(0, ENTRY);
			}
		}

		// sekcja krytyczna
		sleep(1);
		// sekcja krytyczna
		priority += numberOfShips;
		addTowboats(reservedTowboats);
		e_send(0, RELEASE);
		emptyArray(reservedTowboats, numberOfTowboats);
	}
}
