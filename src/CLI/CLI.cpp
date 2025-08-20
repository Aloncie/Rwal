#include "CLI.h"
#include "Menus.h"

void menu(char& choice, int& count){
	for (int i = 0;i < Main_menu.size();i++){
		std::cout << Main_menu[i] << "\n";
	}

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
	
	choice = 0;
	while (choice != '1'&&choice != 'q'){
		count++;
		std::cin.clear();
		std::cin >> choice;
	}
}

void settings(char& choice, int& count, std::string& timer, std::string& wallpaper_local){
	std:: cout << "-------------------------- \n";
	std::cout << "Choose to edit any setting\n";
	std::cout << "1) Timer: " << timer << "\n";
	std::cout << "2) Wallpaper location: " << wallpaper_local << "\n";
	std::cout << "q) Quit\n";
	std:: cout << "-------------------------- \n";

	choice = 0;
	while (choice != '1'&&choice != '2'&&choice != 'q'){
		count++;
		std::cin.clear();
		std::cin >> choice;
	}

}
