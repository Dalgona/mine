CC=clang++ --std=c++11 -O3
CCPARAM=-lcurses

mine: mine.o game.o menu.o
	$(CC) mine.o game.o menu.o -o mine $(CCPARAM)

mine.o:
	$(CC) -c mine.cpp

game.o:
	$(CC) -c game.cpp

menu.o:
	$(CC) -c menu.cpp

clean:
	rm -f *.o
	rm -f mine
