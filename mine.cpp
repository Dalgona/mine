#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ncurses.h>

#include "game.h"
#include "menu.h"
#include "mine.h"

struct score_t
{
  char name[11];
  int time;
};

void begin_arcade(void);
void arcade_leaderboard(void);
void exit_game(void);

std::vector<score_t> scores[3];

int main(void)
{
  menu mainMenu("[ MINESWEEPER ] Select game mode");
  mainMenu.add({ "Custom Mode", "Arcade Mode", "Exit" });
  int sel = mainMenu.start_menu();
  switch (sel)
  {
  case 0:
  {
    int fW, fH;
    printf("Hosizontal size (%d ~ %d)> ", MF_W_MIN, MF_W_MAX);
    fflush(stdout);
    scanf("%d", &fW);
    printf("Vertical size (%d ~ %d)> ", MF_H_MIN, MF_H_MAX);
    fflush(stdout);
    scanf("%d", &fH);

    if (fW < MF_W_MIN) fW = MF_W_MIN;
    if (fW > MF_W_MAX) fW = MF_W_MAX;
    if (fH < MF_H_MIN) fH = MF_H_MIN;
    if (fH > MF_H_MAX) fH = MF_H_MAX;

    game theGame(fH, fW);
    theGame.start();
  } break;
  case 1: begin_arcade(); break;
  case 2: exit_game(); break;
  }

  exit_game();

  return 0;
}

void begin_arcade(void)
{
  menu arcadeMenu("[ MINESWEEPER ] Select difficulty");
  arcadeMenu.add({
    "Beginner (9x9, 10 mines)",
    "Intermediate (16x16, 45 mines)",
    "Expert (16x30, 99 mines)"
  });
  arcadeMenu.draw = arcade_leaderboard;
  int sel = arcadeMenu.start_menu();
  game *theGame;
  switch (sel)
  {
  case 0: theGame = new game(9, 9); break;
  case 1: theGame = new game(16, 16); break;
  case 2: theGame = new game(16, 30); break;
  }
  theGame->start();

  initscr();
  cbreak();
  noecho();

  auto result = theGame->get_result();
  auto &vec = scores[sel];
  if (/*result.win && */result.time < vec[4].time)
  {
    score_t score { "          ", result.time };
    {
      int pos = 0;
      int ch;

      refresh();
      WINDOW *nameWin = newwin(13, 39, 0, 0);
      mvwprintw(nameWin, 0, 5, "C O N G R A T U L A T I O N S");
      mvwprintw(nameWin, 2, 4, "A   N E W   H I G H   S C O R E");
      mvwprintw(nameWin, 4, 0, "What is your name?");
      mvwprintw(nameWin, 8, 0, "Accepted Characters: A-Z, 0-9 and SPACE");
      mvwprintw(nameWin, 12, 5, "BkSp: ERASE     Enter: DECIDE");
      while (true)
      {
        for (int i = 0; i < 10; i++)
          mvwprintw(nameWin, 6, 4 * i, "[%c]", score.name[i]);
        wrefresh(nameWin);
        ch = getch();
        if ((ch >= 'A' && ch <= 'Z')
            || (ch >= '0' && ch <= '9')
            || ch == ' ')
        { if (pos < 10) score.name[pos++] = ch; }
        else if (ch >= 'a' && ch <= 'z')
        { if (pos < 10) score.name[pos++] = ch - 32; }
        else if (ch == 127)
        { if (pos != 0) score.name[--pos] = ' '; }
        else if (ch == 10)
          break;
      }
      delwin(nameWin);
    }
    vec.push_back(score);
    std::stable_sort(vec.begin(), vec.end(), [](const score_t &a, const score_t &b)
      {
        return a.time < b.time;
      });
    std::ofstream data("scores.dat", std::ios::out | std::ios::binary);
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 5; j++)
      {
        data.write(scores[i][j].name, 11);
        data.write((char *)&scores[i][j].time, 4);
      }
    data.close();
  }
  delete theGame;

  clear(); // TODO
  arcade_leaderboard();

  getch();
  endwin();

}

void arcade_leaderboard(void)
{
  std::ifstream data("scores.dat", std::ios::in | std::ios::binary);
  for (int i = 0; i < 3; i++)
  {
    scores[i].resize(5);
    for (int j = 0; j < 5; j++)
    {
      data.read(scores[i][j].name, 11);
      data.read((char *)&scores[i][j].time, 4);
    }
  }
  data.close();

  refresh();
  WINDOW *lbw = newwin(9, 78, 8, 1);

  mvwprintw(lbw, 0, 28, "H A L L   O F   F A M E");

  mvwprintw(lbw, 2, 8, "BEGINNER");
  mvwprintw(lbw, 2, 33, "INTERMEDIATE");
  mvwprintw(lbw, 2, 63, "EXPERT");

  const char *nth[] { "1st", "2nd", "3rd", "4th", "5th" };
  int iPart, dPart;
  for (int i = 0; i < 3; i++)
  {
    int col = 27 * i;
    for (int j = 0; j < 5; j++)
    {
      score_t &sc = scores[i][j];
      if (sc.time > 999999) iPart = 9999, dPart = 99;
      else iPart = sc.time / 100, dPart = sc.time % 100;
      mvwprintw(lbw, 4 + j, col, "%s  %10s  %04d.%02d", nth[j], sc.name, iPart, dPart);
    }
  }

  wrefresh(lbw);
  delwin(lbw);
}

void exit_game(void)
{
  printf("C Minesweeper. Code by Dalgona. <dalgona@hontou.moe>\n");
  exit(0);
}
