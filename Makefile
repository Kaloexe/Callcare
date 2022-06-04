all: callcare call

callcare: callcare.c
	gcc callcare.c call.c -lpthread -o callcare call -l wiringPi -l linphone

call: call.c
	gcc call.c -o call -lwiringPi -llinphone -lpthread
