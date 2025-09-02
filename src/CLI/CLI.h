#pragma once
#include <string>
#include <vector>

struct MenuConfig;

class MenuManager{
private:
	int count_ref;
	static std::vector<std::string> dontShowAgain;
	MenuManager();
public:
	char charactarInput(const MenuConfig& config);
	static MenuManager& getInstatce();
	void clear_last_lines();
	std::string arrowInput(const MenuConfig& config);
	void countOperatorPlus(int count);
	void show_message(std::string message);
	void dodgeMessage(std::string message);

	MenuManager(const MenuManager&) = delete;
	MenuManager& operator=(const MenuManager&) = delete;
};
