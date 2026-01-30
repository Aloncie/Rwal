#include "navigator.hpp"
#include "ui/cli/cli.hpp"


void Navigator::core(const rwal::ui::BaseMenu* current_menu) {
	std::string input;
    do {
		input = (*current_menu).get_input();	
       	auto x = (*current_menu).execute_actions(input); 
		if (x.nextMenu){
			current_menu = x.nextMenu;
			input = "";
		}
		if (x.IsWrongInput)
			MenuManager::getInstatce().show_message("Invalid input");
    } while (input != "q");
}
