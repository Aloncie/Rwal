#include "AppController.hpp"
#include <iostream>
#include "ui/cli/cli.hpp"

AppController::AppController(Navigator* nav, QObject* parent) : QObject(parent), m_navigator(nav)
{
	m_notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
	connect(m_notifier, &QSocketNotifier::activated, this, &AppController::handleStdin);

	m_navigator->printMenu();
}
void AppController::handleStdin(){
	std::string input;
	do {
		do {			
			std::getline(std::cin,input);
			MenuManager::getInstance().countOperatorPlus(1);
		} while (m_navigator->getCurrentMenu()->valid_choices.find(input) == std::string::npos);

		auto menuLines = m_navigator->getCurrentMenu()->menu().size(); 
		MenuManager::getInstance().countOperatorPlus(menuLines);
		MenuManager::getInstance().clear_last_lines();

		m_navigator->processInput(input);
		m_navigator->getCurrentMenu();
		m_navigator->printMenu();
	} while (input != "q");
}

