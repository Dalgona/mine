#pragma once
#ifndef MENU_H_
#define MENU_H_

#include <vector>
#include <string>
#include <initializer_list>
#include <functional>

using menu_action = std::function<void(void)>;

class menu
{
// Fields
private:
  std::string title;
  std::vector<std::string> items;
  std::vector<menu_action> handlers;

// Public Fields
public:
  std::function<void(void)> draw = nullptr;

// Constructors
public:
  menu(std::string);

// Methods
public:
  void add(std::initializer_list<std::string>);
  void start_menu(std::initializer_list<menu_action>);
};

#endif
