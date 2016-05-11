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

void menu::start_menu(std::initializer_list<menu_action> list)
{
  int nModes = items.size();

  for (const auto &i : list)
    handlers.push_back(i);
  if (!nModes) return;
  if (nModes != handlers.size()) return;

  initscr();
  cbreak();
  noecho();
  clear();

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
    if (ch == 10) break;
  }

  endwin();
  menu_action action = handlers[sel];
  if (action != nullptr) action();
}
