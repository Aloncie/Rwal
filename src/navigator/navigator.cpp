#include "navigator.hpp"
#include "ui/cli/cli.hpp"

void Navigator::printMenu(){
	auto menu = current_menu->menu();	
	std::cout << "\n";
	for (int i = 0;i < menu.size();i++){
		std::cout << menu[i] << "\n";
	}
}

void Navigator::processInput(std::string input) {
	auto x = current_menu->execute_actions(input); 
	if (x.nextMenu)
		current_menu = x.nextMenu;
	else if (x.IsWrongInput)
		MenuManager::getInstatce().show_message("Invalid input");
}

const rwal::ui::CharacterMenuConfig* Navigator::getCurrentMenu() const {
	return current_menu;
}
