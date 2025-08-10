#include <string>
#include "net/CurlWrapper.h"
#include "keywords/keywords.h"
#include "dbus/PlasmaDBus.h"
#include "wallpaper/WallpaperManager.h"

class Settings{
	void see_settings();
	void edit_keywords(); //Make with setenv("EDITOR", "/usr/bin/vim", 1);
	void edit_timer();
};

int main(int argc, char *argv[]){
	std::string url;
	std::string local;
	auto keywords = divide_keywords(get_keywords());

	do {
		std::string kw = choose_keyword(keywords);
		MyCurl c("an1CFSaR5hyU5D5AM7lCl66FCzp9Dp4a", kw);
		c.get_request();
		url = c.get_image_url();
		if (!url.empty()) {
			local = c.download_image(url);
			break;
		}
	} while (true);	

	change_wallpaper(argc, argv, local);
	save_wallpaper(local);
	return 0;
}
