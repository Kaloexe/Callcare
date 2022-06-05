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

const char *dest = "mathieu_maes@sip.linphone.org";

pthread_t *ledControllerThread, *callControllerThread;

static void call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg){
        switch(cstate){
                case LinphoneCallOutgoingRinging:
                        printf("Le soignant reçoit l'appel !\n");
                        isRinging = 1;
                        inCall = 0;
                        endCall = 0;
                        isError = 0;
                break;
                case LinphoneCallOutgoingEarlyMedia:
                        printf("Réception de média en avance\n");
                break;
                case LinphoneCallConnected:
                        printf("Connecté !\n");
                        isRinging = 0;
                        inCall = 1;
                        endCall = 0;
                        isError = 0;
                case LinphoneCallStreamsRunning:
                        printf("Mise en place des flux de médias !\n");
                break;
                case LinphoneCallEnd:
                        printf("Appel terminé.\n");
                        inCall = 0;
                        inCall = 0;
                        endCall = 1;
                        isError = 0;
                        linphone_core_terminate_call(lc,call);
                        /*at this stage we don't need the call object */
                        linphone_call_unref(call);
                        linphone_core_destroy(lc);
                        buttonPressing();
                break;
                case LinphoneCallError:
                        printf("Problème lors de l'appel !");
                        isRinging = 0;
                        inCall = 0;
                        endCall = 0;
                        isError = 1;
                        linphone_call_unref(call);
                        linphone_core_destroy(lc);
                        buttonPressing();
                break;
                default:
                        printf("Notification non traitée %i\n",cstate);
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
         La table est remplie avec les rappels
         Permet d'appeller les cas dans call_state_changed
         Permet d'ajouter de la logique pour les états de l'appel
         */
        vtable.call_state_changed=call_state_changed;
        /*
         On instancie le noyau Linphone
        */
        lc=linphone_core_new(NULL,NULL,NULL,NULL);
        
        linphone_core_enable_self_view(lc, TRUE);
        linphone_core_enable_video(lc, TRUE, TRUE);
        
        if (dest){
                /*
                 Passe l'appel vers le destinataire dest
                */
                call=linphone_core_invite(lc,dest);
                if (call==NULL){
                        isError = 1;
                        return 0;
                }
                linphone_call_ref(call);
                
                isRinging = 0;
        }
        
        /* bouble pour recevoir les états de call_state_changed en fond*/
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
                        // Thread gestion des led
                        pthread_join(ledControllerThread, NULL);
                        pthread_join(callControllerThread, NULL);
		}
        }
}

int main(int argc, char **argv){
        
        configPin(); //Configuration des pin GPIO
        wiringPiSetup();
        /* Configuration des entrées sorties */
	pinMode(buttonPin, INPUT);
        
	pinMode(ledPinGreenState, OUTPUT);
	pinMode(ledPinGreenCall, OUTPUT);
        pinMode(ledPinRedError, OUTPUT);
        /* État de base des leds */
        digitalWrite(ledPinGreenState, HIGH);
        digitalWrite(ledPinGreenCall, LOW);
        digitalWrite(ledPinRedError, LOW);
        
        buttonPressing(); 0;
}
