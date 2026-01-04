#include "ui/cli/cli.h"
#include "navigator.h"

MenuManager& m = MenuManager::getInstatce();

void Flows::core(const MenuConfig& current_menu) {
    char choice;
    do {
		//Тут я решил изменить твою логику и работал над этим до мини выгора       		 
        if (current_menu.actions.count(choice)) {
            current_menu.actions.at(choice)();
        } else if (choice != 'q') {
			m.show_message("Invalid choice"); 
		}

    } while (choice != 'q');
}
