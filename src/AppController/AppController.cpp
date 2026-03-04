#include "AppController.hpp"
#include <iostream>
#include "logs/logs.hpp"

AppController::AppController(Navigator* nav, QObject* parent) : QObject(parent), m_navigator(nav)
{
	m_notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
	connect(m_notifier, &QSocketNotifier::activated, this, &AppController::handleStdin);

	m_navigator->printMenu();
}
void AppController::handleStdin(UIManager uimanager){
	std::string input;
	bool needQuit;
	do {			
		input = uimanager.requestInput<std::string>(std::nullopt);
	} while (m_navigator->getCurrentMenu()->valid_choices.find(input) == std::string::npos);

	needQuit = m_navigator->processInput(input, uimanager);
	if (!needQuit)
		m_navigator->printMenu();
}

