#pragma once
#include "ui/menus/menus.hpp"
#include <string>
#include "ui/cli/UIManager.hpp"
#include <map>
#include <memory>

class Navigator{
private:
	std::map<std::string, std::unique_ptr<Menu>> m_menus;
	Menu* m_currentMenu = nullptr;
public:
	void printCurrentMenu();
	void start(const std::string InitialMenu);
	bool processInput(std::string& input, UIManager& uimanager);
	void registerMenu(const std::string& name, std::unique_ptr<Menu> menu);
	std::string getCurrentValidChoices() const;
};
