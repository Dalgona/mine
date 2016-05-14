#pragma once
#ifndef SCREEN_H_
#define SCREEN_H_

#include <functional>
#include <ncurses.h>

class screen
{
// Fields
//private:
public:
  static screen *inst;
  bool enabled = false;
  int rows, cols;

// Constructors
private:
  screen();

// Methods
public:
  static screen *getInstance();
  void enable();
  void disable();
  void updateMaxYX();
  bool isEnabled();
  int getRows();
  int getCols();
  void printw(const char *format, ...);
  void mvprintw(int row, int col, const char *format, ...);
  void with_color(int pair, std::function<void(void)> action);
};

#endif
