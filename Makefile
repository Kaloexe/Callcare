all: call

call: call.c
	gcc call.c -o call -lwiringPi -llinphone -lpthread

clean:
	rm -rf call
