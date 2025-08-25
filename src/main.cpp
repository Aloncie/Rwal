#include "startup_flows/startup_flows.h"
#include "wallpaper/WallpaperManager.h"
#include <string>

int main(int argc,char* argv[]){
	Flows f;
	std::string s;
	if (argv[1])
		s = argv[1];

//	if (s == "--help"  || s  == "-h")
			
	if (s == "--change" || s == "-c")
		refresh_wallpaper(argc,argv,"change");
	else 
		f.core(argc,argv);
	return 0;
}
