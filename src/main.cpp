#include "navigator/navigator.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include <string>
#include <QCoreApplication>
#include <QObject>
#include "ui/menus/menus.hpp"
#include "AppController/AppController.hpp"
#include <iostream>
#include "ui/cli/UIManager.hpp"

int main(int argc,char* argv[]){
	QCoreApplication::setApplicationName("Rwal");
	QCoreApplication::setOrganizationName("Aloncie");

	Navigator navigator(rwal::ui::MAIN_MENU);
	std::string s;
	QCoreApplication app(argc, argv);

	UIManager uimanager;
	uimanager.initUI();

	if (argc > 0&&argv[1])
		s = argv[1];

	if (s == "--change" || s == "-c"){
		refresh_wallpaper("change");
		return 0;
	}

	AppController controller(&navigator);
	int one = app.exec();
	uimanager.shutdownUI();
	return one;
}
