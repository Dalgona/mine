#pragma once
#ifndef GAME_H_
#define GAME_H_

#include <vector>
#include <chrono>

#include "screen.h"

using namespace std::chrono;
using timePoint = time_point<steady_clock>;

struct game_result
{
  bool win;
  int time;
};

class game
{
// Fields
private:
  screen *scr;
  std::vector<int> field;
  int rows, cols;
  int cY = 0, cX = 0;
  int nMines = 0;
  int nFlags = 0;
  int clearValidate = 0;
  bool newGame = true;
  timePoint tBegin, tEnd;
  game_result result;

// Constructors
public:
  game(screen *scr, int r, int c);
  ~game();

// Methods
public:
  void start();
  const game_result &get_result() const;

// Private functions
private:
  int index(int row, int col) const;
  void initColors();
  void updateDisplay();
  void setFlag();
  void setQuestionMark();
  int step(int cy, int cx);
};

#endif
