CC=clang # set to whatever you use.
PARAM=-lcurses

mine: mine.o
	$(CC) mine.o -o mine $(PARAM)

mine.o:
	$(CC) -c mine.c

clean:
	rm -f *.o
	rm mine
