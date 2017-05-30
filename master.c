#include "def.h"

int numberOfShips;

int getIndexByTid(int tid, int tids[]) {
	int i;
	for ( i=0; i<numberOfShips; i++ ) {
		if (tids[i] == tid) {
			return i;
		}
	}
}

double current_timestamp() {
     struct timeval te; 
     gettimeofday(&te, NULL);
     double milliseconds = te.tv_sec*1000 + te.tv_usec/1000;
     return milliseconds;
}

main()
{
	int mytid = pvm_mytid();
	int i;
	int numberOfTowboats;
	int *neededTowboats;

	printf("Podaj liczbe statkow.\n");
	scanf("%d", &numberOfShips);
	printf("Podaj liczbe holownikow.\n");
	scanf("%d", &numberOfTowboats);
	neededTowboats = malloc (sizeof (int) * numberOfTowboats);

	for ( i = 0; i < numberOfShips; i++ ) {
		printf("Podaj liczbe potrzebnych holowników dla statku nr %d.\n", i);
		scanf("%d", &neededTowboats[i]);
	}

	int *tids = malloc (sizeof (int) * numberOfShips);
	int nproc;

	nproc = pvm_spawn(SLAVENAME, NULL, PvmTaskDefault, "", numberOfShips, tids);

	double *prior = malloc (sizeof (double) * numberOfShips);
	for ( i = 0; i < numberOfShips; i++) {
		prior[i] = current_timestamp() + (i*0.1);
	}

	for ( i = 0; i < nproc; i++ )
	{
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&mytid, 1, 1);
		pvm_pkint(&numberOfShips, 1, 1);
		pvm_pkdouble(prior, numberOfShips, 1);
		pvm_pkint(&neededTowboats[i], 1, 1);
		pvm_pkint(tids, numberOfShips, 1);
		pvm_pkint(&numberOfTowboats, 1, 1);
	   	pvm_send(tids[i], MSG_MSTR);
	}

	int counter = 1;
	while(true) {
		int slaveTid, type, receiver, sender;
		double priorit, mypriorit;
		double *priorities = malloc (sizeof (double) * numberOfShips);
		short myActive;
		short *ships = malloc (sizeof (short) * numberOfShips);
		short *tow = malloc (sizeof (short) * numberOfTowboats);
		pvm_recv(-1, RAPORT);
		pvm_upkint(&type, 1, 1);
		pvm_upkint(&slaveTid, 1, 1);
		counter++;
		printf("%s%d ", KWHT, slaveTid);
		switch( type ) {
			case 1:
				{
					printf("Sekcja lokalna\n");
					break;
				}
			case 2:
				{
					printf("Wyslalem REQUEST");
					pvm_upkint(&receiver, 1, 1);
					if(receiver == 0) {
						printf(" do wszystkich\n");
					} else {
						int a = getIndexByTid(receiver, tids);
						printf(" do %d\n", a);
					}
					break;
				}
			case 3:
				{
					printf("Odebralem REQUEST");
					pvm_upkint(&sender, 1, 1);
					printf(" od %d", sender);
					pvm_upkdouble(&priorit, 1, 1);
					printf(" z priorytetem %.1f", priorit);
					pvm_upkdouble(&mypriorit, 1, 1);
					printf(" mam priorytet %.1f", mypriorit);
					pvm_upkshort(&myActive, 1, 1);
					if (myActive) {
						printf(" i jestem aktywny \n");
					} else {
						printf(" i NIE jestem aktywny\n");
					}
					break;
				}
			case 4:
				{
					printf("Wyslalem PERMISSION");
					pvm_upkint(&receiver, 1, 1);
					if(receiver == 0) {
						printf(" do wszystkich\n");
					} else {
						int a = getIndexByTid(receiver, tids);
						printf(" do %d\n", a);
					}
					break;
				}
			case 5:
				{
					printf("%sOdebralem PERMISSION", KBLU);
					pvm_upkint(&sender, 1, 1);
					printf(" od %d\n", sender);
					break;
				}
			case 6:
				{
					printf("%sWyslalem ENTRY", KGRN);
					pvm_upkint(&receiver, 1, 1);
					if(receiver == 0) {
						printf("%s do wszystkich", KGRN);
					} else {
						int a = getIndexByTid(receiver, tids);
						printf("%s do %d", KGRN, a);
					}
					pvm_upkshort(tow, numberOfTowboats, 1);
					printf(" z takimi holownikami");
					for (i=0; i<numberOfTowboats; i++) {
						printf(" %d", tow[i]);
					}
					printf("\n");
					break;
				}
			case 7:
				{
					printf("%sOdebralem ENTRY", KYEL);
					pvm_upkint(&sender, 1, 1);
					printf(" od %d", sender);
					pvm_upkshort(tow, numberOfTowboats, 1);
					printf(" z takimi holownikami");
					for (i=0; i<numberOfTowboats; i++) {
						printf(" %d", tow[i]);
					}
					int next;
					pvm_upkint(&next, 1, 1);
					printf(" i takim next: %d\n", next);
					break;
				}
			case 8:
				{
					printf("Sekcja krytyczna\n");
					break;
				}
			case 9:
				{
					printf("Wyslalem RELEASE");
					pvm_upkint(&receiver, 1, 1);
					if(receiver == 0) {
						printf(" do wszystkich\n");
					} else {
						int a = getIndexByTid(receiver, tids);
						printf(" do %d\n", a);
					}
					break;
				}
			case 10:
				{
					printf("Odebralem RELEASE");
					pvm_upkint(&sender, 1, 1);
					printf(" od %d", sender);
					pvm_upkshort(tow, numberOfTowboats, 1);
					printf(" z takimi holownikami");
					for (i=0; i<numberOfTowboats; i++) {
						printf(" %d", tow[i]);
					}
					printf("\n");
					break;
				}
			case 11:
				{
					printf("Czekam z takimi tablicami priorities");
					pvm_upkdouble(priorities, numberOfShips, 1);
					for (i=0; i<numberOfShips; i++) {
						printf(" %.1f", priorities[i]);
					}
					printf(" i activeShips");
					pvm_upkshort(ships, numberOfShips, 1);
					for (i=0; i<numberOfShips; i++) {
						printf(" %d", ships[i]);
					}
					pvm_upkdouble(&mypriorit, 1, 1);
					printf(" mam priorytet %.1f", mypriorit);
					printf("\n");
					break;
				}
			case 12:
				{
					printf("Rezerwuje holowniki z takimi tablicami availableTowboats");
					pvm_upkshort(ships, numberOfTowboats, 1);
					for (i=0; i<numberOfTowboats; i++) {
						printf(" %d", ships[i]);
					}
					printf(" i reservedTowboats");
					pvm_upkshort(ships, numberOfTowboats, 1);
					for (i=0; i<numberOfTowboats; i++) {
						printf(" %d", ships[i]);
					}
					printf("\n");
					break;
				}
			case 13:
				{
					printf("%sWystapil blad", KRED);
					pvm_upkint(&sender, 1, 1);
					printf("%s statek nr %d", KRED, sender);
					pvm_upkshort(tow, numberOfTowboats, 1);
					printf("%s ma zarezerwowane te same holowniki co ja:", KRED);
					for (i=0; i<numberOfTowboats; i++) {
						printf("%s %d", KRED, tow[i]);
					}
					printf("\n");
					break;
				}
		}
	}
	pvm_exit();
}
