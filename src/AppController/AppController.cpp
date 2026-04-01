#include "AppController.hpp"
#include "logs/logs.hpp"

AppController::AppController(Navigator* nav, UIManager& ui, QObject* parent) : QObject(parent), m_navigator(nav), m_ui(ui)
{
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
        if (ch > 31 && ch < 127) {
            std::string input(1, static_cast<char>(ch));
            resp = m_navigator->processInput(input, m_ui); 
            if (resp.needQuit) return;
        }
    }
	
	//throw away other input
    while(getch() != ERR); 

    m_navigator->printCurrentMenu();
    if (!resp.Message.empty()) {
        m_ui.showMessage(resp.Message);
    }
}
