#include "navigator/navigator.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include <string>
#include <QCoreApplication>
#include <QObject>
#include "ui/menus/menus.hpp"
#include "AppController/AppController.hpp"
#include <iostream>

int main(int argc,char* argv[]){
	Navigator navigator;
	std::string s;
	QCoreApplication app(argc, argv);
	if (argc > 1&&argv[1])
		s = argv[1];

	if (s == "--change" || s == "-c"){
		refresh_wallpaper("change");
		return 0;
	}

	AppController controller(&navigator);
	return app.exec();
}
