/*
 * Copyright (c) 2010-2019 Belledonne Communications SARL.
 *
 * This file is part of Liblinphone.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "call.h"

static bool_t running=TRUE;
static void stop(int signum){
        running=FALSE;
}
static int buttonPin = 27;
static int ledPinGreenCall = 9;
static int ledPinRedError = 21;
static int buttonState = 0;
static int isRinging = 0;
static int inCall = 0;
static int endCall = 0;
static int isError = 0;
const char *dest = "mathieu_maes@sip.linphone.org";
pthread_t *ledControllerThread, *callControllerThread;

static void call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg){
        switch(cstate){
                case LinphoneCallOutgoingRinging:
                        printf("It is now ringing remotely !\n");
                        isRinging = 1;
                break;
                case LinphoneCallOutgoingEarlyMedia:
                        printf("Receiving some early media\n");
                break;
                case LinphoneCallConnected:
                        printf("We are connected !\n");
                        isRinging = 0;
                        inCall = 1;
                case LinphoneCallStreamsRunning:
                        printf("Media streams established !\n");
                break;
                case LinphoneCallEnd:
                        printf("Call is terminated.\n");
                        inCall = 0;
                        inCall = 0;
                        endCall = 1;
                        linphone_core_terminate_call(lc,call);
                        /*at this stage we don't need the call object */
                        linphone_call_unref(call);
                        linphone_core_destroy(lc);
                        buttonPressing();
                break;
                case LinphoneCallError:
                        printf("Call failure !");
                        isRinging = 0;
                        inCall = 0;
                        endCall = 0;
                        isError = 1;
                        linphone_call_unref(call);
                        linphone_core_destroy(lc);
                        buttonPressing();
                break;
                default:
                        printf("Unhandled notification %i\n",cstate);
        }
}

void *ledControl() 
{
        for(;;) {
                if (isError == 1){
                        digitalWrite(ledPinRedError, HIGH);
                }
                
                if (isRinging == 1){ // en attente de réponse
                        digitalWrite(ledPinGreenCall, HIGH);
                        delay(500);
                        digitalWrite(ledPinGreenCall, LOW);
                        delay(500);
                } else if (inCall == 1) { // En appel
                        digitalWrite(ledPinGreenCall, HIGH);
                } else if (endCall == 1) { // Fin de l'appel
                        digitalWrite(ledPinGreenCall, LOW);
                }
                

        }
}

int *call() {
        LinphoneCoreVTable vtable={0};
        LinphoneCore *lc;
        LinphoneCall *call=NULL;
        /* 
         Fill the LinphoneCoreVTable with application callbacks.
         All are optional. Here we only use the call_state_changed callbacks
         in order to get notifications about the progress of the call.
         */
        vtable.call_state_changed=call_state_changed;
        /*
         Instanciate a LinphoneCore object given the LinphoneCoreVTable
        */
        lc=linphone_core_new(&vtable,NULL,NULL,NULL);
        linphone_core_enable_self_view(lc, TRUE);
        linphone_core_enable_video(lc, TRUE, TRUE);
        
        //linphone_core_set_default_sound_devices(lc);
        //linphone_core_reload_sound_devices(lc);
        /*
        char** soundDevices = linphone_core_get_sound_devices(lc);
        int i;
        size_t n = sizeof(soundDevices);
        for(i=0; i<n; i++){
                //printf("\nGet capture device : %s\n", linphone_core_get_sound_device(lc));
                if(strstr(soundDevices[i], "UACDemo")){
                        printf("USB Sound devices :%s\n", soundDevices[i]);
                        //linphone_core_set_capture_device(lc, soundDevices[i]);
                        //printf("\nGet capture device : %s\n", linphone_core_get_capture_device(lc));
                }
        }
        
        */
        if (dest){
                /*
                 Place an outgoing call
                */
                call=linphone_core_invite(lc,dest);
                if (call==NULL){
                        isError = 1;
                        return 0;
                }
                linphone_call_ref(call);
                
                isRinging = 0;
        }
        
        /* main loop for receiving notifications and doing background linphonecore work: */
        while(running){
                linphone_core_iterate(lc);
                
        }
        if (call && linphone_call_get_state(call)!=LinphoneCallEnd){
                /* Fin de l'appel */
                linphone_core_terminate_call(lc,call);
                linphone_call_unref(call);
        }  
}

int buttonPressing(){
        buttonState = digitalRead(buttonPin);
        while(1){
		buttonState = digitalRead(buttonPin);
		if(buttonState == HIGH) {
                        // Thread appel
                        pthread_create(&callControllerThread, NULL, call, NULL);
                        pthread_create(&ledControllerThread, NULL, ledControl, NULL);
                        // Thread led
                        pthread_join(ledControllerThread, NULL);
                        pthread_join(callControllerThread, NULL);
		}
        }
}

int main(int argc, char **argv){
        wiringPiSetup();
        
	pinMode(buttonPin, INPUT);
	pinMode(ledPinGreenCall, OUTPUT);
        pinMode(ledPinRedError, OUTPUT);
        
        digitalWrite(ledPinGreenCall, LOW);
        digitalWrite(ledPinRedError, LOW);
        
        buttonPressing(); 0;
}
