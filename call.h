#include "linphone/core.h"
#include <wiringPi.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
/* Configuration ports GPIO via console
int buttonPin;
int ledPinGreenState;
int ledPinGreenCall;
int ledPinRedError;
*/

static int buttonState = 0;
static int isRinging = 0;
static int inCall = 0;
static int endCall = 0;
static int isError = 0;

extern void configPin();
