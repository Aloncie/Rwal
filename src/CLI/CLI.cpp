#include "CLI.h"

void menu(char& choice, int& count){
	std:: cout << "-------------------------- \n";
	std::cout << "1) Refresh wallpaper now\n";
	std::cout << "2) Save current wallpaper\n";
	std::cout << "3) See keywords\n";
	std::cout << "4) Settings\n";
	std::cout << "q) Quit\n";
	std:: cout << "-------------------------- \n";

	choice = 0;
	while (choice != '1'&&choice != '2'&&choice != '3'&&choice != 'q'){
		count++;
		std::cin.clear();
		std::cin >> choice;
	}
}

void clear_last_lines(int& count) {
    for (int i = 0; i < count; ++i) {
        std::cout << "\033[A\033[2K";
    }
    std::cout << "\r";
	count = 0;
}

void keywords_menu(char& choice, int& count){
	Keywords k;
	std:: cout << "-------------------------- \n";
	std::cout << "Keywords: " << k.look_keywords();
	std::cout << "\n1) Edit keywords\n";
	std::cout << "q) Quit\n";
	std:: cout << "-------------------------- \n";

	choice = 0;
	while (choice != '1'&&choice != 'q'){
		count++;
		std::cin.clear();
		std::cin >> choice;
	}
}

void settings(std::string& timer, std::string& wallpaper_local){
	std:: cout << "-------------------------- \n";
	std::cout << "Choose to edit any setting\n";
	std::cout << "1) Timer: " << timer << "\n";
	std::cout << "2) Wallpaper location: " << wallpaper_local << "\n";
	std::cout << "q) Quit\n";
	std:: cout << "-------------------------- \n";
	
}
