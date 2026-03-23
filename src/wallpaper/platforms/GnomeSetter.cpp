#include "GnomeSetter.hpp"
#include "../../logs/logs.hpp"

#ifdef signals
  #undef signals
#endif

#include <gio/gio.h>

void GnomeSetter::setWallpaper(const fs::path& path) {
    GFile* file = g_file_new_for_path(path.c_str());
    
    if (!file) {
        Logs::getInstance().writeLogs("GnomeSetter: Failed to create GFile from path");
        return;
    }

    char* uri = g_file_get_uri(file);
    GSettings* settings = g_settings_new("org.gnome.desktop.background");

    g_settings_set_string(settings, "picture-uri", uri);
    g_settings_set_string(settings, "picture-uri-dark", uri);

    Logs::getInstance().writeLogs("Wallpaper set to: " + std::string(uri));

    g_free(uri);
    g_object_unref(settings);
    g_object_unref(file);
}

GnomeSetter::GnomeSetter(){
	#if !GLIB_CHECK_VERSION(2,36,0)
	g_type_init();
	#endif
}
