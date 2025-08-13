#include "CLI.h"

int Menu::menu(){
	std:: cout << "-------------------------- \n";
	std::cout << "1) Refresh wallpaper now\n";
	std::cout << "2) Save current wallpaper\n";
	std::cout << "3) See keywords\n";
	std::cout << "4) Quit\n";
	std:: cout << "-------------------------- \n";

	int choice = 0;
	std::cin >> choice;

	if (choice == 1||choice == 2||choice == 3||choice == 4)
		return choice;
}


