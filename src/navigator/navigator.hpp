#pragma once
#include "ui/menus/menus.hpp"
#include "ui/cli/UIManager.hpp"

#include <string>
#include <map>
#include <memory>

class Navigator{
private:
	std::map<std::string, std::unique_ptr<Menu>> m_menus;
	Menu* m_currentMenu = nullptr;
public:
	void printCurrentMenu();
	void start(const std::string InitialMenu);
	MenuResponce processInput(std::string& input, UIManager& uimanager);
	void registerMenu(const std::string& name, std::unique_ptr<Menu> menu);
	const std::string& getCurrentValidChoices() const;
};
