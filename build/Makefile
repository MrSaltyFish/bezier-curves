CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11 -pedantic `pkg-config --cflags sdl2`
LIBS = `pkg-config --libs sdl2`

bezier: ../src/main.c
	$(CC) $(CFLAGS) -o bezier ../src/main.c $(LIBS)

run: bezier
	clear
	./bezier

clean:
	rm -rf bezier
