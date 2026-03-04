#pragma once
#include "ui/menus/menus.hpp"
#include <string>
#include "ui/cli/UIManager.hpp"

class Navigator{
	const rwal::ui::CharacterMenuConfig* current_menu;
public:
	Navigator(const rwal::ui::CharacterMenuConfig& config) : current_menu(&config) {};
	void printMenu();
	bool processInput(std::string input, UIManager uimanager);
	const rwal::ui::CharacterMenuConfig* getCurrentMenu() const;
};
