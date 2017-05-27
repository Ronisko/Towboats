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

	double prior = 0;
	for( i = 0 ; i < nproc ; i++ )
	{
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&mytid, 1, 1);
		pvm_pkint(&numberOfShips, 1, 1);
		pvm_pkdouble(&prior, 1, 1);
		pvm_pkint(&neededTowboats[i], 1, 1);
		pvm_pkint(tids, numberOfShips, 1);
		pvm_pkint(&numberOfTowboats, 1, 1);
	   	pvm_send(tids[i], MSG_MSTR);
		prior++;
	}

	int counter = 1;
	while(true) {
		int slaveTid, type, receiver, sender;
		double priorit, mypriorit;
		short myActive;
		short *ships = malloc (sizeof (short) * numberOfShips);
		short *tow = malloc (sizeof (short) * numberOfTowboats);
		pvm_recv(-1, RAPORT);
		pvm_upkint(&type, 1, 1);
		pvm_upkint(&slaveTid, 1, 1);
		//printf("%d ", counter);
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
					printf(" z priorytetem %.f", priorit);
					pvm_upkdouble(&mypriorit, 1, 1);
					printf(" mam priorytet %.f", mypriorit);
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
					printf("Odebralem ENTRY");
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
					printf("Czekam z takimi tablicami permissions");
					pvm_upkshort(ships, numberOfShips, 1);
					for (i=0; i<numberOfShips; i++) {
						printf(" %d", ships[i]);
					}
					printf(" i activeShips");
					pvm_upkshort(ships, numberOfShips, 1);
					for (i=0; i<numberOfShips; i++) {
						printf(" %d", ships[i]);
					}
					pvm_upkdouble(&mypriorit, 1, 1);
					printf(" mam priorytet %.f", mypriorit);
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

