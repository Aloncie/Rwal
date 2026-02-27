#include "navigator.hpp"
#include "ui/cli/cli.hpp"
#include <QCoreApplication>

void Navigator::printMenu(){
	auto menu = current_menu->menu();	
	for (int i = 0;i < menu.size();i++){
		std::cout << menu[i] << "\n";
	}
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
