#include "navigator.hpp"
#include "logs/logs.hpp"

#include <QCoreApplication>
#include <ncurses.h>

void Navigator::registerMenu(const std::string& name, std::unique_ptr<Menu> menu) {
    m_menus[name] = std::move(menu);
}

const std::string& Navigator::getCurrentValidChoices() const {
    static const std::string empty = "";
    return (m_currentMenu) ? m_currentMenu->getValidChoices() : empty;
}
void Navigator::start(const std::string InitialMenu) {
    auto it = m_menus.find(InitialMenu);
    if (it != m_menus.end()) {
        m_currentMenu = it->second.get();
        printCurrentMenu();
    } else {
        Logs::getInstance().writeLogs("Failed InitialMenu in Navigator::start: " + InitialMenu);
    }
}

void Navigator::printCurrentMenu() {
    if (m_currentMenu) {
        clear();
        int row = 0;
        for (const auto& line : m_currentMenu->getLines()) mvprintw(row++, 0, "%s", line.c_str());
        refresh();
    }
}

MenuResponce Navigator::processInput(std::string& input, UIManager& uimanager) {
    if (!m_currentMenu) return {"", false, false, ""};

    MenuResponce resp = m_currentMenu->handleInput(input);

    if (resp.needQuit) QCoreApplication::quit();

    if (resp.IsWrongInput) uimanager.showMessage("Invalid choice: " + input);

    if (!resp.nextMenu.empty()) {
        auto it = m_menus.find(resp.nextMenu);
        if (it != m_menus.end()) {
            m_currentMenu = it->second.get();
        }
    }

    return resp;
}

