#include "io.hpp"
#include "ui/cli/cli.hpp"
#include "ui/menus/menus.hpp"
#include <iostream>
#include <ncurses.h>

namespace rwal::ui::io{
	std::string CharacterInput(const CharacterMenuConfig& config){
		char choice = ' ';
		std::string input;
		auto menu = config.menu();	
		std::cout << "\n";
		for (int i = 0;i < menu.size();i++){
			std::cout << menu[i] << "\n";
		}

		while (config.valid_choices.find(choice) == std::string::npos){
			std::getline(std::cin,input);
			if (input.length() == 1)
				choice = input[0];
			MenuManager::getInstatce().countOperatorPlus(1);
		}

		MenuManager::getInstatce().countOperatorPlus(menu.size() + 1);

		MenuManager::getInstatce().clear_last_lines();
		return std::string(1, choice);
	}

	std::string ArrowInput(const ArrowMenuConfig& config){
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
}

