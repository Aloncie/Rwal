#pragma once
#include <string>
#include <vector>

class MenuManager{
private:
	int count_ref;
	static std::vector<std::string> dontShowAgain;
	MenuManager();
public:
	static MenuManager& getInstatce();
	void clear_last_lines();
	void countOperatorPlus(int count);
	void show_message(std::string message);
	void dodgeMessage(std::string message);

	MenuManager(const MenuManager&) = delete;
	MenuManager& operator=(const MenuManager&) = delete;
};
