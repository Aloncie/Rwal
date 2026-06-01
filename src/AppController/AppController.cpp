#include "AppController.hpp"
#include "logs/logs.hpp"

#include <ncurses.h>

AppController::AppController(Navigator& nav, TUIManager& tui)
    : m_navigator(nav), m_tui(tui)
{
    m_navigator.printCurrentMenu();
}

bool AppController::handleStdin() {
    int ch = getch();
    if (ch == ERR) return true;  // no input, keep going

    MenuResponse resp;
    if (m_tui.isInputActive()) {
        m_tui.processInputChar(ch);
    } else {
        const std::string validChoices = m_navigator.getCurrentValidChoices();
        char inputChar = static_cast<char>(ch);
        if (validChoices.find(inputChar) != std::string::npos) {
            std::string input(1, static_cast<char>(ch));
            resp = m_navigator.processInput(input, m_tui);
            if (resp.needQuit) return false;  // signal the loop to stop
        }
    }

    m_navigator.printCurrentMenu();
    if (!resp.Message.empty()) {
        m_tui.showMessage(resp.Message);
    }
    return true;  // keep going
}

