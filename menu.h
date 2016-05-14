#pragma once
#ifndef MENU_H_
#define MENU_H_

#include <vector>
#include <string>
#include <initializer_list>
#include <functional>

#include "screen.h"

class menu
{
// Fields
private:
  std::string title;
  std::vector<std::string> items;
  screen *scr;

// Public Fields
public:
  std::function<void(void)> draw = nullptr;

// Constructors
public:
  menu(screen *scr, std::string);

// Methods
public:
  void add(std::initializer_list<std::string>);
  int start_menu(void);
};

#endif
