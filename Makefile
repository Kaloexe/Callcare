all: call

call: call.c
	gcc call.c configCallcare.c -o call -lwiringPi -llinphone -lpthread
