#pragma once
#include "ui/tui/menus/menus.hpp"
#include "logs/logs.hpp"
#include "ui/tui/TUIManager.hpp"

#include <string>
#include <map>
#include <memory>

class Navigator{
private:
	std::map<std::string, std::unique_ptr<Menu>> m_menus;
	Menu* m_currentMenu = nullptr;
	Logs& m_logs;
public:
	Navigator(Logs& logs) : m_logs(logs) {}
	void printCurrentMenu();
	void start(const std::string InitialMenu);
	MenuResponce processInput(std::string& input, TUIManager& ui);
	void registerMenu(const std::string& name, std::unique_ptr<Menu> menu);
	const std::string& getCurrentValidChoices() const;
};

