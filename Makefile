CFLAGS=-Wall -Wextra -std=c11 -Wpedantic
INPUTS= 

.PHONY: ascii_graphics

test: ascii_graphics
	./ascii_graphics $(INPUTS)
	

ascii_graphics: 
	gcc $(CFLAGS) -g -o ascii_graphics src/mesh.c src/draw.c src/matrix.c -lm -lncurses
