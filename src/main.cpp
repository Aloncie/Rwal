#include "navigator/navigator.h"
#include "wallpaper/WallpaperManager.h"
#include <string>
#include <QCoreApplication>
#include "ui/menus/menus.h"


int main(int argc,char* argv[]){
	Navigator n;
	std::string s;
	QCoreApplication app(argc, argv);

	if (argv[1])
		s = argv[1];

//	if (s == "--help"  || s  == "-h")

	if (s == "--change" || s == "-c")
		refresh_wallpaper("change");
	else 
		n.core(&rwal::ui::MAIN_MENU);
	return 0;
}
