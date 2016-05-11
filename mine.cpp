#include <cstdio>
#include <iostream>
#include <ncurses.h>

#include "game.h"
#include "menu.h"
#include "mine.h"

void show_arcade(void);
void exit_game(void);

int main(void)
{
  menu mainMenu("[ MINESWEEPER ] Select game mode");
  mainMenu.add({ "Custom Mode", "Arcade Mode", "Exit" });
  mainMenu.start_menu({
    []() { // Custom Mode Handler
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
    },
    show_arcade, // Arcade Mode Handler
    exit_game,
  });

  exit_game();

  return 0;
}

void show_arcade(void)
{
  menu arcadeMenu("[ MINESWEEPER ] Select difficulty");
  arcadeMenu.add({
    "Beginner (9x9, 10 mines)",
    "Intermediate (16x16, 45 mines)",
    "Expert (16x30, 99 mines)"
  });
  arcadeMenu.start_menu({
    []() { game theGame(9, 9); theGame.start(); },
    []() { game theGame(16, 16); theGame.start(); },
    []() { game theGame(16, 30); theGame.start(); }
  });
}

void exit_game(void)
{
  printf("C Minesweeper. Code by Dalgona. <dalgona@hontou.moe>\n");
  exit(0);
}
