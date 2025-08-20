#include "CLI.h"
#include "Menus.h"
#include "keywords/keywords.h"

void clear_last_lines(int& count) {
    for (int i = 0; i < count; ++i) {
        std::cout << "\033[A\033[2K";
    }
    std::cout << "\r";
	count = 0;
}

MenuManager::MenuManager(int& count) : count_ref(count) {}

char MenuManager::display(const MenuConfig& config){
	char choice;

	for (int i = 0;i < config.visual.size();i++){
		std::cout << config.visual[i] << "\n";
	}

	while (config.valid_choices.find(choice) == std::string::npos){
		count_ref++;
		std::cin.clear();
		std::cin >> choice;
	}

	return choice;
}

char MenuManager::arrowDisplay(const MenuConfig& config){
	initscr();	
	noecho();
	curs_set(0);

	int selected = 0;

	for (int i = 0; i < config.visual.size();i++){
		if (i == selected)

		std::cout << config.visual[i] << "\n";
	}	
	
	
	int key = getch();
	
}
