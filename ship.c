#include "def.h"

void e_send( int to, int message ) {
	pvm_initsend(PvmDataDefault);
	switch( message ) {
		case 1:
			pvm_send(to, PERMISSION);
			break;
		case 2:
			pvm_send(to, REQUEST);
			break;
		case 3:
			pvm_send(to, ENTRY);
			break;
		case 4:
			pvm_send(to, RELEASE);
			break;
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

void deletePermissions(bool *permissions, int number) {
	for (int i = 0; i < number; i++)
	{
		permissions[i] = false;
	}
}

bool equalArrays(int tab[], int tab2[], int length) {
    for (int i = 0; i < length; i++)
        if (tab[i] != tab2[i])
            return false;

    return true;
}

main()
{
	int numberOfShips;
	long long priority;
	int neededTowboats;
	int ships[];

	int mytid = pvm_mytid();
	gethostname(slave_name, NAMESIZE);

	/**
	  * internal
	  */
	pvm_recv( -1, MSG_MSTR );
	pvm_upkint(&numberOfShips, 1, 1);
	pvm_upklong(&priority, 1, 1);
	pvm_upkint(&neededTowboats, 1, 1);
	pvm_upkint(&ships, numberOfShips, 1);
	pvm_upkint(&neededTowboats, 1, 1)

	/* */

	bool permissions[];
	int availableTowboats[];
	int ownedTowboats[];
	int reservedTowboats[];
	bool ready;


	while (true) {
		//sekcja lokalna()

		e_send("REQUEST", neededTowboats, ownedTowboats, priority);
		ready = false;
		deletePermissions(permissions);

		while( !ready ) {
			e_receive();

			if ( equalArrays(zgody, statkiAktywne, ) ) {
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

