#include "def.h"
#include <sys/time.h>

main()
{
	int mytid;
	int tids[SHIPSNUM];
	int neededTowboats[SHIPSNUM];
	int nproc, i, who;

	nproc = pvm_spawn(SHIPNAME, NULL, PvmTaskDefault, "", SHIPSNUM, tids);

	for( i = 0 ; i < nproc ; i++ )
	{
		pvm_initsend(PvmDataDefault);
		pvm_pklong(current_timestamp(), 1, 1);
		pvm_pkint(neededTowboats[i], 1, 1);
		pvm_pkint(&tids, )
	   	pvm_send(tids[i], MSG_MSTR);
	}

	pvm_exit();
}

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}