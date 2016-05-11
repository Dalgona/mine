#include <cstdio>
#include <iostream>
#include <ncurses.h>

#include "game.h"
#include "mine.h"

#define MAXMODE 2
#define MODE_CUSTOM 0
#define MODE_ARCADE 1
#define MODE_EXIT 2

static const char *modeStr[] {
  "Custom Mode", "Arcade Mode", "Exit"
};
static const char *arcadeModeStr[] {
  "Beginner (9x9, 10 mines)",
  "Intermediate (16x16, 45 mines)",
  "Expert (16x30, 99 mines)"
};

int main(void)
{
  int fW, fH;

  initscr();
  cbreak();
  noecho();

  mvprintw(0, 0, "[ MINESWEEPER ] Select game mode");
  int mode = 0;
  for (int i = 0; i <= MAXMODE; i++)
    mvprintw(2 + i, 4, "< > %s", modeStr[i]);
  while (true)
  {
    for (int i = 0; i <= MAXMODE; i++)
      mvaddch(2 + i, 5, ' ');
    mvaddch(2 + mode, 5, '*');
    refresh();
    int ch = getch();
    switch (ch)
    {
    case 'j':
      if (mode < MAXMODE) mode++; break;
    case 'k':
      if (mode > 0) mode--; break;
    }
    if (ch == 10) break;
  }

  switch (mode)
  {
  case MODE_CUSTOM:
  {
    endwin();
    std::cout << "Horizontal size (" << MF_W_MIN << " ~ " << MF_W_MAX << ")> ";
    scanf("%d", &fW);
    std::cout << "Vertical size (" << MF_H_MIN << " ~ " << MF_H_MAX << ")> ";
    scanf("%d", &fH);

    if (fW < MF_W_MIN) fW = MF_W_MIN;
    if (fW > MF_W_MAX) fW = MF_W_MAX;
    if (fH < MF_H_MIN) fH = MF_H_MIN;
    if (fH > MF_H_MAX) fH = MF_H_MAX;

    game theGame(fH, fW);
    theGame.start();
  } break;
  case MODE_ARCADE:
  {
    int mode = 0;
    for (int i = 0; i < 3; i++)
      mvprintw(2 + i, 4, "< > %s", arcadeModeStr[i]);
    while (true)
    {
      for (int i = 0; i < 3; i++)
        mvaddch(2 + i, 5, ' ');
      mvaddch(2 + mode, 5, '*');
      refresh();
      int ch = getch();
      switch (ch)
      {
      case 'j':
        if (mode < 2) mode++; break;
      case 'k':
        if (mode > 0) mode--; break;
      }
      if (ch == 10) break;
    }
    endwin();
    game *theGame;
    switch (mode)
    {
      case 0:
        theGame = new game(9, 9); break;
      case 1:
        theGame = new game(16, 16); break;
      case 2:
        theGame = new game(16, 30); break;
    }
    theGame->start();
    delete theGame;
  } break;
  case MODE_EXIT:
    return 0;
  }

  std::cout << "C Minesweeper. Code by Dalgona. <dalgona@hontou.moe>\n";

  return 0;
}
