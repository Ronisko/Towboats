#include "def.h"

void e_send( int to, char *message ) {



//tutaj cos z pvm
}

void e_receive() {

//tutaj cos z pvm

	if (pcktIn.type == "PERMISSION") {
		zgody.push(Qj); 
	}
	// jeśli otrzymaliśmy "ŻĄDANIE":
	else if (pcktIn.type == "REQUEST") {
		statkiAktywne.push(Qj);
		if (statkiAktywne[i] == true) {
			
			if (pcktIn.priorytet > priorytet) {  
				e_send(Qi, Qj, "PERMISSION");
			}
		}
	}
	// jeśli otrzymaliśmy "ZWOLNIENIE":
	else if (pcktIn.type == "ZWOLNIENIE") {
		holownikiDostępne.push(pcktIn.holowniki); // dodajemy zwolnione holowniki do dostępnych
	}
	// jeśli otrzymaliśmy "WEJŚCIE":
	else if (pcktIn.type == "ENTRY") {
		holownikiDostępne.remove(pcktIn.holowniki) // usuwamy zagarnięte holowniki z dostępnych
		statkiAktywne.remove(Qj); // usuwamy statek będący w sekcji krytycznej z aktywnych
	}
}

void deletePermissions(bool *permissions, int number) {
	for (int i = 0; i < number; i++)
	{
		permissions[i] = false;
	}
}

main()
{
	int neededTowboats;
	char slave_name[NAMESIZE];

	mytid = pvm_mytid();
	gethostname(slave_name, NAMESIZE);

	pvm_recv( -1, MSG_MSTR );
	pvm_upkint(&first, 1, 1);
	pvm_upkint(&tid, 1, 1 );
	pvm_upklong(&neededTowboats, 1, 1)

	bool permissions[];
	int availableTowboats[];
	int ownedTowboats[];
	int reservedTowboats[];
	bool activeShips;
	int priority;
	bool ready;


	while (true) {
		//sekcja lokalna()
		e_send("REQUEST", neededTowboats, ownedTowboats, priority);
		ready = false;
		deletePermissions(permissions);

		while( !ready ) {
		e_receive();

		if ( zgody.length == statkiAktywne.length ) {
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

