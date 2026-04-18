#include "AppController.hpp"
#include "logs/logs.hpp"

#include <ncurses.h>
#include <QApplication>

AppController::AppController(Navigator& nav, TUIManager& ui, QObject* parent)
    : QObject(parent), m_navigator(nav), m_ui(ui) {
#ifdef Q_OS_UNIX
    m_notifier = std::make_unique<QSocketNotifier>(STDIN_FILENO, QSocketNotifier::Read, this);
    connect(m_notifier.get(), &QSocketNotifier::activated, this, &AppController::handleStdin);
#elif defined(Q_OS_WIN)
    m_notifier = std::make_unique<QWinEventNotifier>(GetStdHandle(STD_INPUT_HANDLE), this);
    connect(m_notifier.get(), &QWinEventNotifier::activated, this, [this](HANDLE) { handleStdin(); });
#endif    
    m_navigator.printCurrentMenu();
}

void AppController::handleStdin() {
    int ch = getch();
    if (ch == ERR) return;

    MenuResponce resp;
    if (m_ui.isInputActive()) {
        m_ui.processInputChar(ch);
    } else {
        const std::string validChoices = m_navigator.getCurrentValidChoices();
        char inputChar = static_cast<char>(ch);
        if (validChoices.find(inputChar) != std::string::npos) {
            std::string input(1, static_cast<char>(ch));
            resp = m_navigator.processInput(input, m_ui);
            if (resp.needQuit) {
				QCoreApplication::quit();
				return;
			}
        }
    }

    m_navigator.printCurrentMenu();
    if (!resp.Message.empty()) {
        m_ui.showMessage(resp.Message);
    }
}

