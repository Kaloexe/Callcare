#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int buttonPin = 27;
int buttonState = 0;
int actionBouton = 1;

void affichage()
{
	if (actionBouton) {
		printf("Button pressed\n");
		actionBouton = 0;
		sleep(1);
		actionBouton = 1;
	}
}

void main()
{
	wiringPiSetup();
	pinMode(buttonPin, INPUT);
	
	while (1) {
		buttonState = digitalRead(buttonPin);
		
		if(buttonState == HIGH) {
	
			affichage();
		}
	}
	return;
}
