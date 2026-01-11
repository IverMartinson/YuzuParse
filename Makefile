COMPILER=gcc
FLAGS_ALL=-g -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-sequence-point
FLAGS_EXAMPLE=-Lbuilds/ -lyuzuparse -Wl,-rpath=builds/ -lm
FLAGS_LIB=-fPIC -shared -lc -lm  -lportaudio

main.bin: libyuzuparse.so
	$(COMPILER) $(FLAGS_ALL) src/launch\ program/main.c -o builds/main.bin $(FLAGS_EXAMPLE) 

libyuzuparse.so:
	$(COMPILER) $(FLAGS_ALL) src/main/main.c -o builds/libyuzuparse.so $(FLAGS_LIB) 

clean:
	rm builds/*
