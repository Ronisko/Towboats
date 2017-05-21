#include "def.h"

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
		for (i=0; i < numberOfShips) {
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
	int towboats[];
	if ( pvm_nrecv(-1, PERMISSION) ) {
		pvm_upkint(&j, 1, 1);
		zgody[j] = true; 
	}
	else if ( pvm_nrecv(-1, REQUEST) ) {
		pvm_upkint(&j, 1, 1);
		pvm_upkint(&receivedPriority, 1, 1);
		statkiAktywne[j] = true;
		if (statkiAktywne[i] == true) {
			
			if (receivedPriority > priority) {  
				e_send(Qi, Qj, PERMISSION);
			}
		}
	}
	else if ( pvm_nrecv(-1, REQUEST) ) {
		pvm_upkint(towboats, SIZE, 1);
		holownikiDostępne.push(pcktIn.holowniki);
	}
	else if ( pvm_nrecv(-1, ENTRY) ) {
		pvm_upkint(&j, 1, 1);
		holownikiDostępne.remove(pcktIn.holowniki)
		statkiAktywne[j] = false;
	}
}

void deletePermissions(bool permissions[], int length) {
	for (int i = 0; i < length; i++)
		permissions[i] = false;
}

bool equalArrays(int permissions[], int acti[], int length) {
    for (int i = 0; i < length; i++)
        if (tab[i] != tab2[i])
            return false;

    return true;
}

main()
{
	int numberOfShips; // set
	long long priority; // set
	int neededTowboats; // set
	int ships[5]; // set
	int numberOfTowboats; // set

	int mytid = pvm_mytid();

	/**
	  * internal
	  */
	pvm_recv( -1, MSG_MSTR );
	pvm_upkint(&numberOfShips, 1, 1);
	pvm_upklong(&priority, 1, 1);
	pvm_upkint(&neededTowboats, 1, 1);
	pvm_upkint(&ships, numberOfShips, 1);
	pvm_upkint(&numberOfTowboats, 1, 1)

	/* */

	bool permissions[numberOfShips];
	bool availableTowboats[numberOfTowboats];
	bool ownedTowboats[numberOfTowboats];
	bool reservedTowboats[numberOfTowboats];
	bool ready;

	while (true) {
		//sekcja lokalna()

		e_send("REQUEST", neededTowboats, ownedTowboats, priority);
		ready = false;
		deletePermissions(permissions, numberOfShips);

		while( !ready ) {
			e_receive();

			if ( equalArrays(permissions, statkiAktywne, numberOfShips) ) {
				gotowy = true;
				while ( true ) {
					holownikiZarezerwowane = holownikiDostępne.slice ( liczbaPotrzebnychHolowników )
					if ( liczbaPotrzebnychHolowników != holownikiZarezerwowane.length() ) {
						break; 
					}
					e_receive();
				}
				holownikiKtoreMam.push(holownikiZarezerwowane);
				holownikiDostępne.remove(holownikiZarezerwowane);
				e_send(Qi, *, "ENTRY");
			} 
		}
	}

}
