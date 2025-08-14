#include "CLI.h"

int menu(){
	std:: cout << "-------------------------- \n";
	std::cout << "1) Refresh wallpaper now\n";
	std::cout << "2) Save current wallpaper\n";
	std::cout << "3) See keywords\n";
	std::cout << "4) Quit\n";
	std:: cout << "-------------------------- \n";

	int choice = 0;
	std::cin >> choice;

	return choice;
}

void clear_last_lines(int count) {
    for (int i = 0; i < count; ++i) {
        std::cout << "\033[A\033[2K";
    }
    std::cout << "\r";
}


