CC=clang # set to whatever you use.

mine: mine.o getch.o
	$(CC) mine.o getch.o -o mine

mine.o:
	$(CC) -c mine.c

getch.o:
	$(CC) -c getch.c

clean:
	rm -f *.o
