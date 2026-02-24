#include "AppController.hpp"
#include <iostream>
#include "ui/cli/cli.hpp"

AppController::AppController(Navigator* nav, QObject* parent = nullptr) : QObject(parent), m_navigator(nav)
{
	m_notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
	connect(m_notifier, &QSocketNotifier::activated, this, &AppController::handleStdin);
}
void AppController::handleStdin(){
	std::string input;
	do {

	while (config.valid_choices.find(input) == std::string::npos){
		std::getline(std::cin,input);
		MenuManager::getInstatce().countOperatorPlus(1);
	}

	MenuManager::getInstatce().countOperatorPlus(menu.size());

	MenuManager::getInstatce().clear_last_lines();
	m_navigator->processInput(input);
	m_navigator->getCurrentMenu();
	m_navigator->printMenu();
	} while (input != "q");
}

#include "AppController.moc"
