CC=clang # set to whatever you use.
CPPC=clang++ --std=c++11
CCPARAM=-lcurses

mine: mine.o
	$(CC) mine.o -o mine $(CCPARAM)

mine.o:
	$(CC) -c mine.c

cppmine: cppmine.o cppgame.o
	$(CPPC) mine.o game.o -o mine $(CCPARAM)

cppmine.o:
	$(CPPC) -c mine.cpp

cppgame.o:
	$(CPPC) -c game.cpp

clean:
	rm -f *.o
	rm mine
