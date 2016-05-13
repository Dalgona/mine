#include <ncurses.h>
#include "menu.h"

menu::menu(std::string title)
  : title(title)
{ }

void menu::add(std::initializer_list<std::string> list)
{
  for (const auto &i : list)
    items.push_back(i);
}

int menu::start_menu(void)
{
  int nModes = items.size();

  if (!nModes) return -1;

  initscr();
  cbreak();
  noecho();
  clear();
  curs_set(0);

  mvprintw(0, 0, "%s", title.c_str());

  for (int i = 0; i < nModes; i++)
    mvprintw(2 + i, 4, "< > %s", items[i].c_str());

  if (draw != nullptr) draw();

  int sel = 0;
  while (true)
  {
    for (int i = 0; i < nModes; i++)
      mvaddch(2 + i, 5, ' ');
    mvaddch(2 + sel, 5, '*');
    refresh();
    int ch = getch();
    switch (ch)
    {
    case 'j':
      if (sel < nModes - 1) sel++; break;
    case 'k':
      if (sel > 0) sel--; break;
    }
    if (ch == 10)
    {
      curs_set(1);
      endwin();
      return sel;
    }
  }
}
