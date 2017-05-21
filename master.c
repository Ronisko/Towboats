#include "def.h"

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

main()
{
	int i;
	int numberOfShips = 5;
	int numberOfTowboats = 10;

	// printf("Podaj liczbe statkow.\n");
	// scanf("%d", &numberOfShips);
	// for ( i = 0; i < numberOfShips; i++ ) {
	// 	printf("Podaj liczbe potrzebnych holowników dla statku nr %d.\n", i);
	// 	scanf("%d", &numberOfShips);
	// }

	int tids[5];
	int neededTowboats[5] = {5, 8, 2, 5, 10};
	int nproc;

	nproc = pvm_spawn(SHIPNAME, NULL, PvmTaskDefault, "", numberOfShips, tids);

	for( i = 0 ; i < nproc ; i++ )
	{
		pvm_initsend(PvmDataDefault);
		pvm_pkint(numberOfShips, 1, 1);
		pvm_pklong(current_timestamp(), 1, 1);
		pvm_pkint(neededTowboats[i], 1, 1);
		pvm_pkint(&tids, numberOfShips, 1);
		pvm_pkint(numberOfTowboats, 1, 1);
	   	pvm_send(tids[i], MSG_MSTR);
	}

	pvm_exit();
}

