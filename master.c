#include "def.h"

double current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    double milliseconds = te.tv_sec*1000 + te.tv_usec/1000; // caculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

main()
{
	int mytid = pvm_mytid();
	int i;
	int numberOfShips = 5;
	int numberOfTowboats = 10;
	char msgRaport[5];

	// printf("Podaj liczbe statkow.\n");
	// scanf("%d", &numberOfShips);
	// for ( i = 0; i < numberOfShips; i++ ) {
	// 	printf("Podaj liczbe potrzebnych holowników dla statku nr %d.\n", i);
	// 	scanf("%d", &numberOfShips);
	// }

	int tids[5];
	int neededTowboats[5] = {5, 8, 2, 5, 10};
	int nproc;

	nproc = pvm_spawn(SLAVENAME, NULL, PvmTaskDefault, "", numberOfShips, tids);

	double prior = current_timestamp();
	for( i = 0 ; i < nproc ; i++ )
	{
		prior++;
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&mytid, 1, 1);
		pvm_pkint(&numberOfShips, 1, 1);
		pvm_pkdouble(&prior, 1, 1);
		pvm_pkint(&neededTowboats[i], 1, 1);
		pvm_pkint(tids, numberOfShips, 1);
		pvm_pkint(&numberOfTowboats, 1, 1);
	   	pvm_send(tids[i], MSG_MSTR);
	}
	while(true) {
		int slaveTid;
		pvm_recv(-1, RAPORT);
		pvm_upkint(&slaveTid, 1, 1);
		pvm_upkstr(msgRaport);
		printf("Slave nr: %d w fazie: %s\n", slaveTid, msgRaport);
	}
	pvm_exit();
}
