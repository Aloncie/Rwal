#include <algorithm>
#include <cctype>
#include <map>
#include <ncurses.h>
#include "cli.h"
#include <iostream>

std::vector<std::string> MenuManager::dontShowAgain;

void MenuManager::clear_last_lines() {
    for (int i = 0; i < count_ref; ++i) {
        std::cout << "\033[A\033[2K";
    }
    std::cout << "\r";
	count_ref = 0;
}

extern std::map<std::string,int> countStr;

MenuManager::MenuManager() : count_ref(0) {}

MenuManager& MenuManager::getInstatce(){
	static MenuManager instance;
	return instance;
}

void MenuManager::countOperatorPlus(int count){
	count_ref += count;
}

void MenuManager::show_message(std::string message){
	if (std::find(dontShowAgain.begin(),dontShowAgain.end(),message) != dontShowAgain.end())
		return;

	std::string cp = message;
	std::transform(cp.begin(),cp.end(),cp.begin(), ::tolower);
	if (cp.find("successful") != std::string::npos)
		//green
		std::cout << "\033[1;32m" << message << "\033[0m" << std::endl;
	else if (cp.find("failed") != std::string::npos||cp.find("error") != std::string::npos||cp.find("invalid") != std::string::npos)
		//red
   		std::cout << "\033[1;31m" << message << "\033[0m" << std::endl;
	else 
		std::cout << message << std::endl;
	count_ref++;
	for (int i = 0; i < message.size(); i++){
		if (message[i] == '\n'){
			count_ref++;
		}
	}
}

void MenuManager::dodgeMessage(std::string message){
	if (std::find(dontShowAgain.begin(),dontShowAgain.end(),message) != dontShowAgain.end())
		return;
	dontShowAgain.push_back(message);
}

