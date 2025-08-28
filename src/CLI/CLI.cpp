#include "CLI.h"
#include <algorithm>
#include <cctype>

void MenuManager::clear_last_lines() {
    for (int i = 0; i < count_ref; ++i) {
        std::cout << "\033[A\033[2K";
    }
    std::cout << "\r";
	count_ref = 0;
}

MenuManager::MenuManager(int& count) : count_ref(count) {}

char MenuManager::charactarInput(const MenuConfig& config){
	char choice;
	auto menu = config.menu();	
	for (int i = 0;i < menu.size();i++){
		std::cout << menu[i] << "\n";
	}

	while (config.valid_choices.find(choice) == std::string::npos){
		count_ref++;
		std::cin.clear();
		std::cin >> choice;
	}

	count_ref+= menu.size();

	clear_last_lines();
	return static_cast<char>(choice);
}

std::string MenuManager::arrowInput(const MenuConfig& config){
	initscr();		
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);	

	int old_y, old_x;
	getyx(stdscr,old_y,old_x);
	
	bool running = true;
	int selected = 0;
	auto menu = config.menu();

	while (running){
		
		for (int i = 0; i < menu.size();i++){
			move(old_y + i, 0);
			clrtoeol();
		}

		for (int i = 0; i < menu.size();i++){
			move(old_y + i,0);	
			if (i == selected){
				attron(A_REVERSE);
				printw("> %s", menu[i].c_str());
				attroff(A_REVERSE);
			}
			else 
				printw(" %s", menu[i].c_str());
			clrtoeol();
		}	
		
		move(old_y, old_x);	
		refresh();

		int key = getch();
		switch(key) {
			case KEY_UP: selected = std::max(0,selected -1); break;
			case KEY_DOWN: selected = std::min((int)menu.size()-1, selected+1);break;
			case 10: running = false; break;
		}
	}

	endwin();
	return menu[selected];
}

void MenuManager::countOperatorPlus(int count){
	count_ref += count;
}

void MenuManager::show_message(std::string message){
	std::string cp = message;
	std::transform(cp.begin(),cp.end(),cp.begin(), ::tolower);
	if (cp.find("failed") == std::string::npos&&cp.find("error") == std::string::npos)
		//green
		std::cout << "\033[1;32m" << message << "\033[0m" << std::endl;
	else
		//red
   		std::cout << "\033[1;31m" << message << "\033[0m" << std::endl;
}
