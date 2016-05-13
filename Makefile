CC=clang++ --std=c++11 -O3
CCPARAM=-lcurses

all: mine reset-scores

mine: mine.o game.o menu.o
	$(CC) mine.o game.o menu.o -o mine $(CCPARAM)

mine.o:
	$(CC) -c mine.cpp

game.o:
	$(CC) -c game.cpp

menu.o:
	$(CC) -c menu.cpp

reset-scores:
	$(CC) reset-scores.cpp -o reset-scores

clean:
	rm -f *.o
	rm -f mine
	rm -f reset-scores
