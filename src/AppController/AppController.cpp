#include "AppController.hpp"
#include <iostream>
#include "logs/logs.hpp"

AppController::AppController(Navigator* nav, UIManager& ui, QObject* parent) : QObject(parent), m_navigator(nav), m_ui(ui)
{
	m_notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
	connect(m_notifier, &QSocketNotifier::activated, this, &AppController::handleStdin);

	m_navigator->printCurrentMenu();
}
void AppController::handleStdin(){
	std::string input;
	bool needQuit;
	std::string valid = m_navigator->getCurrentValidChoices();
	do {			
		input = m_ui.requestInput<std::string>(std::nullopt);
	} while (valid.find(input) == std::string::npos);

	needQuit = m_navigator->processInput(input, m_ui);
	if (!needQuit)
		m_navigator->printCurrentMenu();
}

