#include "navigator.hpp"
#include <QCoreApplication>
#include <ncurses.h>
#include "logs/logs.hpp"

void Navigator::registerMenu(const std::string& name, std::unique_ptr<Menu> menu) {
    m_menus[name] = std::move(menu);
}

std::string Navigator::getCurrentValidChoices() const {
	if (m_currentMenu)
		return m_currentMenu->getValidChoices();
	return "";
}
void Navigator::start(const std::string InitialMenu){
	auto it = m_menus.find(InitialMenu);
	if (it != m_menus.end()){
		m_currentMenu = it->second.get();
		printCurrentMenu();
	}
	else
		Logs::getInstance().write_logs("Failed InitialMenu in Navigator::start: " + InitialMenu);
}

void Navigator::printCurrentMenu() {
    if (m_currentMenu) {
        clear();
        int row = 0;
        for (const auto& line : m_currentMenu->getLines()) {
            mvprintw(row++, 0, "%s", line.c_str());
        }
        refresh();
    }
}

bool Navigator::processInput(std::string& input, UIManager& uimanager) {
	if (!m_currentMenu)
		return true;
	MenuResponce resp = m_currentMenu->handleInput(input);

	if (resp.needQuit){
		QCoreApplication::quit();
		return true;
	}

	else if (resp.IsWrongInput){
		uimanager.showMessage("Failed input. Try again");
		return false;	
	}

    if (!resp.nextMenu.empty()) {
        auto it = m_menus.find(resp.nextMenu);
        if (it != m_menus.end()) {
            m_currentMenu = it->second.get();
        } else 
			Logs::getInstance().write_logs("Error: unknown menu name");
	}
	return false;
}

