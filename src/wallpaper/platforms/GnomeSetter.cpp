#include "GnomeSetter.hpp"
#include "logs/logs.hpp"

#include <gio/gio.h>
bool GnomeSetter::setWallpaper(const fs::path& path) {
    if (std::getenv("XDG_CURRENT_DESKTOP") == nullptr ||
        std::string(std::getenv("XDG_CURRENT_DESKTOP")).find("GNOME") == std::string::npos) {
        Logs::getInstance().writeLogs(
            "Skipping GnomeSetter: XDG_CURRENT_DESKTOP does not indicate GNOME.");
        return false;
    }
    if (!g_file_test(path.c_str(), G_FILE_TEST_EXISTS)) {
        Logs::getInstance().writeLogs("GnomeSetter: File does not exist - " + path.string());
        return false;
    }
    GFile* file = g_file_new_for_path(path.c_str());

    if (!file) {
        Logs::getInstance().writeLogs("GnomeSetter: Failed to create GFile from path");
        return false;
    }

    char* uri = g_file_get_uri(file);
    GSettings* settings = g_settings_new("org.gnome.desktop.background");

    g_settings_set_string(settings, "picture-uri", uri);
    g_settings_set_string(settings, "picture-uri-dark", uri);
    g_settings_sync();

    Logs::getInstance().writeLogs("Wallpaper set to: " + std::string(uri));

    g_free(uri);
    g_object_unref(settings);
    g_object_unref(file);
    return true;
}

GnomeSetter::GnomeSetter() {
#if !GLIB_CHECK_VERSION(2, 36, 0)
    g_type_init();
#endif
}

