#include "navigator/navigator.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include <string>
#include <QCoreApplication>
#include <QObject>
#include "ui/menus/menus.hpp"
#include "AppController/AppController.hpp"
#include <iostream>
#include "ui/cli/UIManager.hpp"
#include "keywords/keywords.hpp"
#include "logs/logs.hpp"
#include "settings/settings.hpp"
#include "settings/config.hpp"

int main(int argc,char* argv[]){
	QCoreApplication::setApplicationName("Rwal");
	QCoreApplication::setOrganizationName("Aloncie");
	QCoreApplication app(argc, argv);

	UIManager uimanager;
	Config config;
	Keywords keywords(uimanager);
	Timer timer;


	if (argc > 1 && (strcmp(argv[1], "--change") == 0 || strcmp(argv[1], "-c") == 0)) {
		refresh_wallpaper(keywords, "change");
		return 0;
	}

	auto mainMenu = std::make_unique<MainMenu>(uimanager, keywords);
    auto settingsMenu = std::make_unique<SettingsMenu>(timer);
    auto keywordsMenu = std::make_unique<KeywordsMenu>(keywords, uimanager);
    auto timerMenu = std::make_unique<TimerMenu>(timer);

	Navigator navigator;
    navigator.registerMenu("main", std::move(mainMenu));
    navigator.registerMenu("settings", std::move(settingsMenu));
    navigator.registerMenu("keywords", std::move(keywordsMenu));
    navigator.registerMenu("timer", std::move(timerMenu));

	navigator.start("main");

	uimanager.initUI();
	AppController controller(&navigator, uimanager);
	Logs::init(uimanager);
	Logs::getInstance().write_logs("Rwal started");

	int one = app.exec();
	uimanager.shutdownUI();
	return one;
}
