#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ncurses.h>
#include <sys/file.h>

#include "screen.h"
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
void name_input(score_t &score);
void initColors();
void exit_game(void);

screen *scr;
std::vector<score_t> scores[3];

int main(void)
{
  scr = screen::getInstance();
  scr->enable();
  initColors();

  menu mainMenu(scr, "[ MINESWEEPER ] Select game mode");
  mainMenu.add({ "Custom Mode", "Arcade Mode", "Exit" });
  int sel = mainMenu.start_menu();
  switch (sel)
  {
  case 0:
  {
    scr->disable();

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

    scr->enable();
    game theGame(scr, fH, fW);
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
  menu arcadeMenu(scr, "[ MINESWEEPER ] Select difficulty");
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
  case 0: theGame = new game(scr, 9, 9); break;
  case 1: theGame = new game(scr, 16, 16); break;
  case 2: theGame = new game(scr, 16, 30); break;
  }
  theGame->start();

  initscr();
  cbreak();
  noecho();

  auto result = theGame->get_result();
  auto &vec = scores[sel];
  if (result.win && result.time < vec[4].time)
  {
    score_t score { "          ", result.time };
    name_input(score);
    vec.push_back(score);
    std::stable_sort(vec.begin(), vec.end(), [](const score_t &a, const score_t &b)
      {
        return a.time < b.time;
      });
    FILE *data = fopen("scores.dat", "wb");
    int fd = fileno(data);
    flock(fd, LOCK_EX);
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 5; j++)
      {
        fwrite(scores[i][j].name, 11, 1, data);
        fwrite(&scores[i][j].time, 4, 1, data);
      }
    flock(fd, LOCK_UN);
    fclose(data);
  }
  delete theGame;

  clear();
  arcade_leaderboard();

  getch();
}

#define LBC(x, y) scr->with_color(lbw, (x), [&]() { y });
void arcade_leaderboard(void)
{
  FILE *data = fopen("scores.dat", "rb");
  int fd = fileno(data);
  flock(fd, LOCK_EX);
  for (int i = 0; i < 3; i++)
  {
    scores[i].resize(5);
    for (int j = 0; j < 5; j++)
    {
      fread(scores[i][j].name, 11, 1, data);
      fread(&scores[i][j].time, 4, 1, data);
    }
  }
  flock(fd, LOCK_UN);
  fclose(data);

  refresh();
  WINDOW *lbw = newwin(9, 78, 8, 1);

  wattron(lbw, A_BOLD);
  LBC(14, mvwprintw(lbw, 0, 27, " H A L L   O F   F A M E "););
  LBC(15, mvwprintw(lbw, 2, 8, "BEGINNER"););
  LBC(16, mvwprintw(lbw, 2, 33, "INTERMEDIATE"););
  LBC(17, mvwprintw(lbw, 2, 63, "EXPERT"););
  wattroff(lbw, A_BOLD);

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
      LBC(20 + j, mvwprintw(lbw, 4 + j, col, "%s", nth[j]););
      wprintw(lbw, "  %10s  %04d.%02d", sc.name, iPart, dPart);
    }
  }

  wrefresh(lbw);
  delwin(lbw);
}
#undef LBC

#define NIC(x, y) scr->with_color(nameWin, (x), [&]() { y });
void name_input(score_t &score)
{
  int pos = 0;
  int ch;

  curs_set(0);
  WINDOW *nameWin;
  auto drawForm = [&]()
  {
    clear();
    refresh();
    nameWin = newwin(13, 39, scr->getRows() / 2 - 6, scr->getCols() / 2 - 19);
    wattron(nameWin, A_BOLD);
    NIC(18, mvwprintw(nameWin, 0, 5, "C O N G R A T U L A T I O N S"););
    NIC(19, mvwprintw(nameWin, 2, 4, "A   N E W   H I G H   S C O R E"););
    wattroff(nameWin, A_BOLD);
    NIC(11, mvwprintw(nameWin, 4, 0, "What is your name?");
           mvwprintw(nameWin, 8, 0, "Accepted Characters: A-Z, 0-9 and SPACE"););
    NIC(23, mvwprintw(nameWin, 12, 5, "BkSp: ERASE     Enter: DECIDE"););
  };
  drawForm();

  while (true)
  {
    for (int i = 0; i < pos; i++)
      NIC(31, mvwprintw(nameWin, 6, 4 * i, " %c ", score.name[i]););
    for (int i = pos; i < 10; i++)
      NIC(30, mvwprintw(nameWin, 6, 4 * i, " %c ", score.name[i]););
    wrefresh(nameWin);
    ch = getch();
    if ((ch >= 'A' && ch <= 'Z')
        || (ch >= '0' && ch <= '9')
        || ch == ' ')
    { if (pos < 10) score.name[pos++] = ch; }
    else if (ch >= 'a' && ch <= 'z')
    { if (pos < 10) score.name[pos++] = ch - 32; }
    else if (ch == 8 || ch == KEY_BACKSPACE)
    { if (pos != 0) score.name[--pos] = ' '; }
    else if (ch == KEY_RESIZE)
    { scr->updateMaxYX(); delwin(nameWin); drawForm(); }
    else if (ch == 10)
      break;
  }

  delwin(nameWin);
  curs_set(1);
}

void initColors(void)
{
  init_pair(11, 255, 0);    // white on black
  init_pair(12, 234, 234);  // menu unselected
  init_pair(13, 82, 82);    // menu selected
  init_pair(14, 123, 0);    // leaderboard title
  init_pair(15, 82, 0);     // beginner
  init_pair(16, 172, 0);    // intermediate
  init_pair(17, 198, 0);    // expert
  init_pair(18, 227, 0);    // name title 1
  init_pair(19, 214, 0);    // name title 2
  init_pair(20, 227, 0);    // 1st
  init_pair(21, 254, 0);    // 2nd
  init_pair(22, 178, 0);    // 3rd
  init_pair(23, 243, 0);    // 4th
  init_pair(24, 243, 0);    // 5th
  init_pair(30, 238, 234);  // name blank
  init_pair(31, 123, 17);   // name char
}

void exit_game(void)
{
  scr->disable();
  printf("C Minesweeper. Code by Dalgona. <dalgona@hontou.moe>\n");
  exit(0);
}
