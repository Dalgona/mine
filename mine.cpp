#include <cstdio>
#include <iostream>

#include "game.h"
#include "mine.h"

int main(void)
{
  int fW, fH;

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

  std::cout << "C Minesweeper. Code by Dalgona. <dalgona@hontou.moe>\n";

  return 0;
}
