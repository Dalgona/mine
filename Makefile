CC=clang++ --std=c++11
CCPARAM=-lcurses

mine: mine.o game.o
	$(CC) mine.o game.o -o mine $(CCPARAM)

mine.o:
	$(CC) -c mine.cpp

game.o:
	$(CC) -c game.cpp

clean:
	rm -f *.o
	rm -f mine
