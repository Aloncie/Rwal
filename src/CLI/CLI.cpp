#include "CLI.h"
#include "Menus.h"
#include "keywords/keywords.h"

void MenuManager::clear_last_lines() {
    for (int i = 0; i < count_ref; ++i) {
        std::cout << "\033[A\033[2K";
    }
    std::cout << "\r";
	count_ref = 0;
}

MenuManager::MenuManager(int& count) : count_ref(count) {}

char MenuManager::display(const MenuConfig& config){
	char choice;

	for (int i = 0;i < config.menu.size();i++){
		std::cout << config.menu[i] << "\n";
	}

	while (config.valid_choices.find(choice) == std::string::npos){
		count_ref++;
		std::cin.clear();
		std::cin >> choice;
	}
	return static_cast<char>(choice);
}

char MenuManager::arrowDisplay(const MenuConfig& config){
	initscr();		
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);	

	int old_y, old_x;
	getyx(stdscr,old_y,old_x);
	
	bool running = true;
	int selected = 0;

	while (running){
		
		for (int i = 0; i < config.menu.size();i++){
			move(old_y + i, 0);
			clrtoeol();
		}

		for (int i = 0; i < config.menu.size();i++){
			move(old_y + i,0);	
			if (i == selected){
				attron(A_REVERSE);
				printw("> %s", config.menu[i].c_str());
				attron(A_REVERSE);
			}
			else 
				printw(" %s", config.menu[i].c_str());
			clrtoeol();
		}	
		
		move(old_y, old_x);	
		refresh();

		int key = getch();
		switch(key) {
			case KEY_UP: selected = std::max(0,selected -1); break;
			case KEY_DOWN: selected = std::min((int)config.menu.size()-1, selected+1);break;
			case 10: running = false; break;
		}
	}

	endwin();
	return static_cast<char>(selected);

}
