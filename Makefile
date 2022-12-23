CFLAGS=-Wall -Wextra -std=c11 -Wpedantic
INPUTS= 

.PHONY: test
test: terminal_graphics
	./terminal_graphics $(INPUTS)
	


terminal_graphics: mesh.c draw.c
	gcc $(CFLAGS) -g -o terminal_graphics mesh.c draw.c matrix.c -lm -lncurses
