all: testlib

testlib: testlib.o primlib.o
	gcc -fsanitize=undefined -g $^ -o $@  -lSDL2_gfx `sdl2-config --libs` -lm

.c.o: 
	gcc -g -fsanitize=undefined -Wall -pedantic `sdl2-config --cflags` -c  $<

bsearch: bsearch.o primlib.o
	gcc -fsanitize=undefined -g $^ -o $@  -lSDL2_gfx `sdl2-config --libs` -lm

primlib.o: primlib.c primlib.h

bsearch.o: bsearch.c primlib.h

testlib.o: testlib.c primlib.h

clean:
	-rm primlib.o testlib.o testlib
