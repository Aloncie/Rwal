#include "AppController.hpp"
#include <iostream>
#include "ui/cli/cli.hpp"
#include "logs/logs.hpp"

AppController::AppController(Navigator* nav, QObject* parent) : QObject(parent), m_navigator(nav)
{
	m_notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
	connect(m_notifier, &QSocketNotifier::activated, this, &AppController::handleStdin);

	m_navigator->printMenu();
}
void AppController::handleStdin(){
	std::string input;
	bool needQuit;
	do {			
		input = MenuManager::getInstance().request_input<std::string>(std::nullopt);
	} while (m_navigator->getCurrentMenu()->valid_choices.find(input) == std::string::npos);

	auto menuLines = m_navigator->getCurrentMenu()->menu().size(); 
	needQuit = m_navigator->processInput(input);
	if (!needQuit)
		m_navigator->printMenu();
}

