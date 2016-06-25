#include <ncurses.h>
#include "menu.h"

menu::menu(screen *scr, std::string title)
  : scr(scr), title(title)
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
  if (!scr->isEnabled()) return -1;

  clear();
  curs_set(0);

  scr->with_color(11, [&]() {
    attron(A_BOLD);
    scr->mvprintw(0, 0, "%s", title.c_str());
    attroff(A_BOLD);
  });

  for (int i = 0; i < nModes; i++)
    scr->mvprintw(2 + i, 8, "%s", items[i].c_str());

  if (draw != nullptr) draw();

  int sel = 0;
  while (true)
  {
    for (int i = 0; i < nModes; i++)
      scr->with_color(12, [&]() { scr->mvprintw(2 + i, 4, "   "); });
    scr->with_color(13, [&]() { scr->mvprintw(2 + sel, 4, "   "); });
    refresh();
    int ch = wgetch(stdscr);
    switch (ch)
    {
    case KEY_DOWN:
    case 'j':
      if (sel < nModes - 1) sel++; break;
    case KEY_UP:
    case 'k':
      if (sel > 0) sel--; break;
    case KEY_RESIZE:
      scr->updateMaxYX();
      if (draw != nullptr) draw();
      break;
    case KEY_ENTER:
    case '\n':
      curs_set(1);
      return sel;
    }
  }
}
