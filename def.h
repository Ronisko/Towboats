#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


#define SLAVENAME "slave"

#define NAMESIZE   64

#define MSG_MSTR 1
#define PERMISSION 2
#define REQUEST 3
#define ENTRY 4
#define RELEASE 5

