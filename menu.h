#pragma once
#ifndef MENU_H_
#define MENU_H_

#include <vector>
#include <string>
#include <initializer_list>
#include <functional>

class menu
{
// Fields
private:
  std::string title;
  std::vector<std::string> items;

// Public Fields
public:
  std::function<void(void)> draw = nullptr;

// Constructors
public:
  menu(std::string);

// Methods
public:
  void add(std::initializer_list<std::string>);
  int start_menu(void);
};

#endif
