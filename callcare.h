#include "call.h"
#include <wiringPi.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int call_linphone(char *dest_arg);


typedef struct {
	LinphoneCore *lc;
	LinphoneCall *call;
	const char *dest;
} linphone_call_struct;

