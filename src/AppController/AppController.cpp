#include "AppController.hpp"
#include "logs/logs.hpp"

AppController::AppController(Navigator* nav, TUIManager& ui, QObject* parent)
    : QObject(parent), m_navigator(nav), m_ui(ui) {
    m_notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &AppController::handleStdin);
    m_navigator->printCurrentMenu();
}

void AppController::handleStdin() {
    int ch = getch();
    if (ch == ERR) return;

    MenuResponce resp;
    if (m_ui.isInputActive()) {
        m_ui.processInputChar(ch);
    } else {
        const std::string validChoices = m_navigator->getCurrentValidChoices();
        char inputChar = static_cast<char>(ch);
        if (validChoices.find(inputChar) != std::string::npos) {
            std::string input(1, static_cast<char>(ch));
            resp = m_navigator->processInput(input, m_ui);
            if (resp.needQuit) return;
        }
    }

    while (getch() != ERR);

    m_navigator->printCurrentMenu();
    if (!resp.Message.empty()) {
        m_ui.showMessage(resp.Message);
    }
}

