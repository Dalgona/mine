#include <algorithm>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>

#include "mine.h"
#include "game.h"

game::game(int r, int c)
  : rows(r), cols(c)
{
  field = std::vector<int>(rows * cols);
  std::fill(field.begin(), field.end(), MF_CLEAN | MF_DISP_UNKNOWN);
}

game::~game() { }

void game::start()
{
  // Initialize ncurses mode
  initscr();
  getmaxyx(stdscr, screen.rows, screen.cols);
  cbreak();
  noecho();
  colorOn();

  updateDisplay();

  std::srand(std::time(nullptr));

  // Main game loop
  while (true)
  {
    int key = getch();
    int result = 0;

    switch (key)
    {
    case 'h': cX = cX == 0 ? cols - 1 : cX - 1; break;
    case 'j': cY = cY == rows - 1 ? 0 : cY + 1; break;
    case 'k': cY = cY == 0 ? rows - 1 : cY - 1; break;
    case 'l': cX = cX == cols - 1 ? 0 : cX + 1; break;

    case 'f': setFlag();             break;
    case 'q': setQuestionMark();     break;
    case 's': result = step(cY, cX); break;

    case KEY_RESIZE:
      getmaxyx(stdscr, screen.rows, screen.cols);
      break;
    }
    // end of switch statement

    // Game Over
    if (result == 0xFF)
    {
      for (auto &i : field) if (i & MF_MINE) i = MF_DISP_MINE;
      updateDisplay();
      attron(COLOR_PAIR(2));
      mvprintw(screen.rows - 2, 0, "GAME OVER! You stepped on a mine!\n");
      clrtoeol();
      mvprintw(screen.rows - 1, 0, "Press any key to exit...");
      attroff(COLOR_PAIR(2));
      getch();
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
        attron(COLOR_PAIR(3));
        mvprintw(screen.rows - 2, 0, "CONGRATULATIONS! You've found all mine!\n");
        clrtoeol();
        mvprintw(screen.rows - 1, 0, "Press any key to continue...");
        attroff(COLOR_PAIR(3));
        getch();
        break;
      }
    }

    updateDisplay();
  }

  // Finalize ncurses mode
  endwin();
}

int game::index(int row, int col) const { return row * cols + col; }

void game::setFlag()
{
  int &cp = field[index(cY, cX)];

  if (cp & _MF_DISP_UNKNOWN)
  {
    cp &= ~_MF_DISP_UNKNOWN;
    cp |= _MF_DISP_FLAG;
    nFlags++;
  }
  else if (cp & _MF_DISP_QUESTION)
  {
    cp &= ~_MF_DISP_QUESTION;
    cp |= _MF_DISP_FLAG;
    nFlags++;
  }
  else if (cp & _MF_DISP_FLAG)
  {
    cp &= ~_MF_DISP_FLAG;
    cp |= _MF_DISP_UNKNOWN;
    nFlags--;
  }
}

void game::setQuestionMark()
{
  int &cp = field[index(cY, cX)];

  if (cp & _MF_DISP_UNKNOWN)
  {
    cp &= ~_MF_DISP_UNKNOWN;
    cp |= _MF_DISP_QUESTION;
  }
  else if (cp & _MF_DISP_FLAG)
  {
    cp &= ~_MF_DISP_FLAG;
    cp |= _MF_DISP_QUESTION;
    nFlags--;
  }
  else if (cp & _MF_DISP_QUESTION)
  {
    cp &= ~_MF_DISP_QUESTION;
    cp |= _MF_DISP_UNKNOWN;
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
    // FIXME: currently the number of mines is also determined at random.
    for (i = 0; i < std::rand() % rows + std::rand() % 6 + 1; i++)
      for (j = 0; j < std::rand() % cols + std::rand () % 6 + 1; j++)
        field[index(std::rand() % rows, std::rand() % cols)] |= MF_MINE;
    if (cp & MF_MINE) cp &= ~MF_MINE;

    // Count the number of mines
    for (const auto &i : field) if (i & MF_MINE) nMines++;
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
  int i, j;
  int num;
  int color;
  char ch;

  clear();
  mvprintw(1, 0, "size: %d", field.size());
  mvprintw(0, 0, "[ MINESWEEPER ] %03d mines out of %03d (%02d, %02d)", nFlags, nMines, cX, cY);
  if (newGame)
  {
    attron(COLOR_PAIR(4));
    mvprintw(2, 8, "Step anywhere to start the game!");
    attroff(COLOR_PAIR(4));
  }

  // TODO: optimize (field is a 1-dimension vector)
  for (i = 0; i < rows; i++)
  {
    move(4 + i, 8);
    for (j = 0; j < cols; j++)
    {
      if      (field[index(i, j)] & MF_DISP_UNKNOWN)   color = 200, ch = '.';
      else if (field[index(i, j)] & MF_DISP_FLAG)      color = 201, ch = 'F';
      else if (field[index(i, j)] & MF_DISP_QUESTION)  color = 202, ch = '?';
      else if (field[index(i, j)] & MF_DISP_EMPTY)     color = 203, ch = ' ';
      else if (field[index(i, j)] & MF_DISP_NUMBER)
      {
        num = (field[index(i, j)] % 0x10000000) / 0x01000000;
        color = 100 + num, ch = '0' + num;
      }
      else if (field[index(i, j)] & MF_DISP_MINE)      color = 204, ch = '@';
      else if (field[index(i, j)] & MF_DISP_MINE_CLR)  color = 205, ch = '@';

      attron(COLOR_PAIR(color));
      addch(ch);
      attroff(COLOR_PAIR(color));
    }
  }

  attron(COLOR_PAIR(1));
  mvprintw(screen.rows - 1, 0, "ARROW/hjkl: MOVE    f: FLAG    q: NOTSURE    s: STEP");
  attroff(COLOR_PAIR(1));

  move(4 + cY, 8 + cX);

  refresh();
}

void game::colorOn()
{
  if (has_colors())
  {
    start_color();
    init_pair(1, 255, 0);      // Guide text
    init_pair(2, 197, 0);      // Game over message
    init_pair(3, 118, 0);      // Game cleared message
    init_pair(4, 86, 0);       // New game message
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
}