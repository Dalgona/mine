#include <cstdarg>
#include <ncurses.h>
#include "screen.h"

screen *screen::inst = nullptr;

screen::screen() { };

screen *screen::getInstance()
{
  if (inst == nullptr)
    inst = new screen();
  return inst;
}

void screen::enable()
{
  if (enabled) return;
  initscr();
  cbreak();
  noecho();
  updateMaxYX();
  if (has_colors())
    start_color();
  enabled = true;
}

void screen::disable()
{
  if (!enabled) return;
  endwin();
  enabled = false;
}

void screen::updateMaxYX() { getmaxyx(stdscr, rows, cols); }

bool screen::isEnabled() { return enabled; }
int screen::getRows() { return rows; }
int screen::getCols() { return cols; }

void screen::printw(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vwprintw(stdscr, format, args);
  va_end(args);
}

void screen::mvprintw(int row, int col, const char *format, ...)
{
  va_list args;
  va_start(args, format);

  if (row < 0) row = rows + row;
  if (col < 0) col = cols + col;
  wmove(stdscr, row, col);
  vwprintw(stdscr, format, args);

  va_end(args);
}

void screen::with_color(int pair, std::function<void(void)> action)
{
  attron(COLOR_PAIR(pair));
  action();
  attroff(COLOR_PAIR(pair));
}
