all:compile 

compile: 
	gcc src/*.c -o player -I include/ -lm -lfftw3 -lSDL2 -lSDL2_image -lpthread 
