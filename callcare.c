#include "callcare.h"

int call_linphone(char *dest_arg);
const int buttonPin = 27;
const int ledPin = 9;

static bool_t running=TRUE;
static void stop(int signum){
running=FALSE;
}

void *appel(char *dest)
{
	printf("Fonction appel\n");
	call_linphone(*dest);
	
	linphone_call_struct *call_struct = args;
	printf("Création structure...\n");
	char *dest = call_struct->dest;
	LinphoneCore *lc = call_struct->lc;
	LinphoneCall *call = call_struct->call;
	printf("Fin création structure...\n");
	//free(call_struct);
	/*
	if (dest){
		printf("if dest...\n");
		/*
		belle_sip_object_pool_t * belle_sip_object_pool_push(void) {
			belle_sip_list_t *pools = get_current_pool_stack(NULL);
			belle_sip_object_pool_t *pool;
			pool=belle_sip_object_pool_new();
			*pools = belle_sip_list_prepend(*pools, pool);
			return pool;
		}
		*/
		/* Appel vers l'adresse sip passée en paramètre */
		/*
		call=linphone_core_invite(lc,dest);
		printf("call...\n");
		if (call==NULL){
				printf("Could not place call to %s\n",dest);
				return 0;
				goto end;
		} else printf("Call to %s is in progress...",dest);
		linphone_call_ref(call);
	}
	*/
	/* main loop for receiving notifications and doing background linphonecore work: */
	/*
	while(running){
		
			digitalWrite(ledPin, HIGH);
			linphone_core_iterate(lc);
			//ms_usleep(50000);
	}
	*/
	/*
	if (call && linphone_call_get_state(call)!=LinphoneCallEnd){
			printf("Terminating the call...\n");
			linphone_core_terminate_call(lc,call);
			//at this stage we don't need the call object
			linphone_call_unref(call);
	}  
	*/
	end: 
	printf("Shutting down...\n");
	digitalWrite(ledPin, LOW);
	//linphone_core_destroy(lc);
	pthread_exit(0);
}

int main(int argc, char **argv)
{
	pthread_t thread_appel;
	int buttonState;
	const char *dest = NULL;
	linphone_call_struct args;
	/* Mise en place du bouton et de la LED */
	wiringPiSetup();
	pinMode(buttonPin, INPUT);
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);
	
	/* Mise en place des varaibles Linphone */
	//linphone_call_struct *args = malloc(sizeof *args);
	
	LinphoneCore *lc;
	LinphoneCall *call=NULL;
	
	/* take the destination sip uri from the command line arguments */
	if (argc>1){
			//dest=argv[1];
			args->dest = argv[1];
	}
	signal(SIGINT,stop);
	
	/*
	 Instanciate a LinphoneCore object
	*/
	args->lc=linphone_core_new(NULL,NULL,NULL,NULL);
		
	/* Lecture appui sur le bouton */
	while(1){
		buttonState = digitalRead(buttonPin);
		if(buttonState == HIGH) {
			
			args->lc = &lc;
			args->call = &call;
			args->dest = &dest;
			
			/* Lancement du thread d'appel */
			pthread_create(&thread_appel, NULL, appel, (linphone_call_struct *) args);
			break;
		}
	}
	//pthread_join(thread_appel, NULL);

	return 0;
}
