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

int main(int argc,char* argv[]){
	QCoreApplication::setApplicationName("Rwal");
	QCoreApplication::setOrganizationName("Aloncie");
	QCoreApplication app(argc, argv);

	Navigator navigator(rwal::ui::MAIN_MENU);
	UIManager uimanager;
	Keywords keywords(uimanager);

	uimanager.initUI();
	Logs::init(uimanager);

	std::string s;
	if (argc > 0&&argv[1])
		s = argv[1];

	if (s == "--change" || s == "-c"){
		refresh_wallpaper("change");
		return 0;
	}

	AppController controller(&navigator, uimanager);

	Logs::getInstance().write_logs("Rwal started");

	int one = app.exec();
	uimanager.shutdownUI();
	return one;
}
