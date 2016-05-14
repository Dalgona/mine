CC=clang++ --std=c++11 -O3
CCPARAM=-lcurses

all: mine reset-scores

debug:
	clang++ --std=c++11 -g mine.cpp game.cpp menu.cpp screen.cpp -lcurses -o mine

mine: mine.o game.o menu.o screen.o
	$(CC) mine.o game.o menu.o screen.o -o mine $(CCPARAM)

mine.o:
	$(CC) -c mine.cpp

game.o:
	$(CC) -c game.cpp

menu.o:
	$(CC) -c menu.cpp

screen.o:
	$(CC) -c screen.cpp

reset-scores:
	$(CC) reset-scores.cpp -o reset-scores

clean:
	rm -f *.o
	rm -f mine
	rm -f reset-scores
