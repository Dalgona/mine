#include <algorithm>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <ratio>
#include <ncurses.h>

#include "mine.h"
#include "game.h"

game::game(screen *scr, int r, int c)
  : scr(scr), rows(r), cols(c)
{
  field = std::vector<int>(rows * cols);
  std::fill(field.begin(), field.end(), MF_CLEAN | MF_DISP_UNKNOWN);
}

game::~game() { }

void game::start()
{
  // Initialize ncurses mode
  if (!scr->enabled) return;

  initColors();

  clear();
  updateDisplay();

  std::srand(std::time(nullptr));

  // Main game loop
  while (true)
  {
    int key = getch();
    int result = 0;

    switch (key)
    {
    case KEY_LEFT:  case 'h': cX = cX == 0 ? cols - 1 : cX - 1; break;
    case KEY_DOWN:  case 'j': cY = cY == rows - 1 ? 0 : cY + 1; break;
    case KEY_UP:    case 'k': cY = cY == 0 ? rows - 1 : cY - 1; break;
    case KEY_RIGHT: case 'l': cX = cX == cols - 1 ? 0 : cX + 1; break;

    case 'f': setFlag();             break;
    case 'q': setQuestionMark();     break;
    case 's': result = step(cY, cX); break;

    case KEY_RESIZE:
      scr->updateMaxYX();
      clear();
      break;
    }

    // Game Over
    if (result == 0xFF)
    {
      for (auto &i : field) if (i & MF_MINE) i = MF_DISP_MINE;
      updateDisplay();
      scr->with_color(2, [&]() {
        scr->mvprintw(-3, 0, "GAME OVER! You stepped on a mine!");
      });
      this->result.win = false;
      break;
    }
    // Game Cleared
    if (!newGame && nMines == nFlags)
    {
      clearValidate = 0;
      for (const auto &i : field)
        if (i & MF_MINE && i & MF_DISP_FLAG) clearValidate++;
      if (clearValidate == nMines)
      {
        for (auto &i : field) if (i & MF_MINE) i = MF_DISP_MINE_CLR;
        updateDisplay();
        scr->with_color(3, [&]() {
          scr->mvprintw(-3, 0, "CONGRATULATIONS! You've found all mine!");
        });
        this->result.win = true;
        break;
      }
    }

    updateDisplay();
  }

  tEnd = steady_clock::now();
  int elapsed = this->result.time =  (int)duration<double, std::centi>(tEnd - tBegin).count();
  scr->mvprintw(-2, 0, "Elapsed time: %04d.%02d seconds\n", elapsed / 100, elapsed % 100);
  clrtoeol();
  {
    scr->mvprintw(-1, 0, "Press ENTER to exit...");
    int key;
    while (true)
    {
      key = getch();
      if (key == KEY_ENTER || key == '\n') break;
    }
  }

  clear();
}

const game_result &game::get_result() const { return this->result; }

int game::index(int row, int col) const { return row * cols + col; }

void game::setFlag()
{
  int &cp = field[index(cY, cX)];

  if (cp & MF_DISP_UNKNOWN)
  {
    cp &= ~MF_DISP_UNKNOWN;
    cp |= MF_DISP_FLAG;
    nFlags++;
  }
  else if (cp & MF_DISP_QUESTION)
  {
    cp &= ~MF_DISP_QUESTION;
    cp |= MF_DISP_FLAG;
    nFlags++;
  }
  else if (cp & MF_DISP_FLAG)
  {
    cp &= ~MF_DISP_FLAG;
    cp |= MF_DISP_UNKNOWN;
    nFlags--;
  }
}

void game::setQuestionMark()
{
  int &cp = field[index(cY, cX)];

  if (cp & MF_DISP_UNKNOWN)
  {
    cp &= ~MF_DISP_UNKNOWN;
    cp |= MF_DISP_QUESTION;
  }
  else if (cp & MF_DISP_FLAG)
  {
    cp &= ~MF_DISP_FLAG;
    cp |= MF_DISP_QUESTION;
    nFlags--;
  }
  else if (cp & MF_DISP_QUESTION)
  {
    cp &= ~MF_DISP_QUESTION;
    cp |= MF_DISP_UNKNOWN;
  }
}

int game::step(int cy, int cx)
{
  int count = 0;
  int i, j;
  int &cp = field[index(cy, cx)];

  if (newGame)
  {
    newGame = false;
    // Plant mines
    long double x = std::log(2.15561e+9);
    long double ratio = std::log(rows * cols) / x - 0.08101856l; // logarithmic
    //long double ratio = 0.000207502l * rows * cols + 0.106649146l; // linear
    nMines = (int)lround(ratio * rows * cols);
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, rows * cols - 1);
    int idx, c = 0;

    while (true)
    {
      idx = dist(rd);
      if (idx == index(cy, cx) || field[idx] & MF_MINE) continue;
      field[idx] |= MF_MINE;
      c++;
      if (c == nMines) break;
    }

    clear();
    tBegin = steady_clock::now();
  }

  if (cp & MF_DISP_UNKNOWN)
    if (cp & MF_MINE) return 0xFF; // When the player stepped on a mine
    else
    {
      for (i = cy - 1; i <= cy + 1; i++)
        for (j = cx - 1; j <= cx + 1; j++)
          if (i < 0 || i >= rows || j < 0 || j >= cols) continue;
          else if (field[index(i, j)] & MF_MINE) count++;
      if (count == 0)
      {
        cp &= ~MF_DISP_UNKNOWN;
        cp |= MF_DISP_EMPTY;
        for (i = cy - 1; i <= cy + 1; i++)
          for (j = cx - 1; j <= cx + 1; j++)
            if (i < 0 || i >= rows || j < 0 || j >= cols) continue;
            else step(i, j);
      }
      else
      {
        cp &= ~MF_DISP_UNKNOWN;
        cp |= MF_DISP_NUMBER;
        cp |= (count << 24);
      }
      return 0;
    }
  else return -1;
}

void game::updateDisplay()
{
  scr->mvprintw(0, 0, "[ MINESWEEPER ] ");
  if (newGame)
  {
    scr->with_color(4, [&]() {
      scr->printw("Step anywhere to start the game!");
    });
  }
  else
    scr->printw("%03d mines out of %03d (%02d, %02d)", nFlags, nMines, cX, cY);

  int num, color, c = 0;
  std::div_t d;
  char ch;
  for (const auto &i : field)
  {
    d = std::div(c++, cols);
    if (d.rem == 0) move(2 + d.quot, 8);

    if      (i & MF_DISP_UNKNOWN)  color = 200, ch = '.';
    else if (i & MF_DISP_FLAG)     color = 201, ch = 'F';
    else if (i & MF_DISP_QUESTION) color = 202, ch = '?';
    else if (i & MF_DISP_EMPTY)    color = 203, ch = ' ';
    else if (i & MF_DISP_NUMBER)
    {
      num = (i & 0x0FFFFFFF) >> 24;
      color = 100 + num, ch = '0' + num;
    }
    else if (i & MF_DISP_MINE)     color = 204, ch = '@';
    else if (i & MF_DISP_MINE_CLR) color = 205, ch = '@';

    scr->with_color(color, [&]() { addch(ch); });
  }

  scr->with_color(1, [&]() {
    scr->mvprintw(-1, 0, "ARROW/hjkl: MOVE    f: FLAG    q: NOTSURE    s: STEP");
  });
  move(2 + cY, 8 + cX);
  refresh();
}

void game::initColors()
{
  init_pair(1, 255, 0);      // Guide text
  init_pair(2, 197, 0);      // Game over message
  init_pair(3, 118, 0);      // Game cleared message
  init_pair(4, 86, 0);       // New game message
  /* pairs 11~100 are reserved */
  init_pair(101, 21, 250);   // Number 1
  init_pair(102, 22, 250);   // Number 2
  init_pair(103, 88, 250);   // Number 3
  init_pair(104, 18, 250);   // Number 4
  init_pair(105, 100, 250);  // Number 5
  init_pair(106, 24, 250);   // Number 6
  init_pair(107, 240, 250);  // Number 7
  init_pair(108, 240, 250);  // Number 8
  init_pair(109, 240, 250);  // Number 9
  init_pair(200, 250, 253);  // Unexplored
  init_pair(201, 196, 253);  // Flag
  init_pair(202, 235, 253);  // Question
  init_pair(203, 250, 250);  // Blank
  init_pair(204, 226, 202);  // Explosion
  init_pair(205, 235, 28);   // Correct
}
