#include "navigator.hpp"
#include "ui/cli/cli.hpp"
#include <QCoreApplication>
#include <ncurses.h>

void Navigator::printMenu(){
    clear();
    int row = 0;
    for (const auto& line : current_menu->menu()) {
        mvprintw(row++, 0, "%s", line.c_str());
    }
    refresh();
}
bool Navigator::processInput(std::string input) {
	auto x = current_menu->execute_actions(input); 
	if (x.nextMenu)
		current_menu = x.nextMenu;
	else if (x.IsWrongInput)
		MenuManager::getInstance().show_message("Invalid input");
	else if (x.needQuit){
		QCoreApplication::quit();
		return true;
	}
	return false;
}

const rwal::ui::CharacterMenuConfig* Navigator::getCurrentMenu() const {
	return current_menu;
}
