#include "GnomeSetter.hpp"
#include "logs/logs.hpp"
#include <gio/gio.h>

void GnomeSetter::setWallpaper(const fs::path& path){
	GFile* file = g_file_new_for_path(path);
	char* uri = g_file_get_uri(file);
	GSettings* settings = g_settings_new("org.gnome.desktop.background");
	const char* key = "picture-uri";

	if (g_settings_set_string(settings, key, uri)){
		Logs::getInstance().writeLogs("Wallpaper set to: " + uri);	
		g_settings_sync();
	}
	else{
		Logs::getInstance().writeLogs("Failed to set wallpaper");
	}

	g_free(uri);
	g_object_unref(file);
	g_object_unref(settings);
}

GnomeSetter::GnomeSetter(){
	#if !GLIB_CHECK_VERSION(2,36,0)
	g_type_init();
	#endif
}
