#pragma once
#ifndef GAME_H_
#define GAME_H_

#include <vector>

class game
{
// Fields
private:
  std::vector<int> field;
  int rows, cols;
  int cY = 0, cX = 0;
  int nMines = 0;
  int nFlags = 0;
  int clearValidate = 0;
  bool newGame = true;
  struct { int rows; int cols; } screen;

// Constructors
public:
  game(int r, int c);
  ~game();

// Methods
public:
  void start();

// Private functions
private:
  int index(int row, int col) const;
  void colorOn();
  void updateDisplay();
  void setFlag();
  void setQuestionMark();
  int step(int cy, int cx);
};

#endif
