#include "AppController.hpp"
#include <iostream>
#include "logs/logs.hpp"

AppController::AppController(Navigator* nav, UIManager& ui, QObject* parent) : QObject(parent), m_navigator(nav), m_ui(ui)
{
	m_notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
	connect(m_notifier, &QSocketNotifier::activated, this, &AppController::handleStdin);

	m_navigator->printCurrentMenu();
}

void AppController::handleStdin() {
    int ch;
    while ((ch = getch()) != ERR) {
        if (m_ui.isInputActive()) {
            m_ui.processInputChar(ch);
        } else {
            std::string input(1, static_cast<char>(ch));
            bool needQuit = m_navigator->processInput(input, m_ui);
            if (!needQuit)
                m_navigator->printCurrentMenu();
        }
    }
}

