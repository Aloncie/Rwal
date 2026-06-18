#include "GnomeSetter.hpp"

#include <dlfcn.h>
#include <memory>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

GnomeSetter::GnomeSetter(Logs& logs) : m_logs(logs) {
    m_gioLoaded = loadGio();
    if (m_gioLoaded) {
        m_logs.writeLogs(lvl::Info, mod::Wallpaper, "Loaded GIO library.");
    } else {
        m_logs.writeLogs(lvl::Error, mod::Wallpaper, "Failed to load GIO library.");
    }
}

GnomeSetter::~GnomeSetter() {
    unloadGio();
}

bool GnomeSetter::loadGio() {
    m_gioHandle = dlopen("libgio-2.0.so.0", RTLD_NOW);
    if (!m_gioHandle) {
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper, std::string("GnomeSetter: dlopen failed: ") + dlerror());
        return false;
    }

    dlerror(); // clear previous errors

    g_settings_new = reinterpret_cast<GSettingsNew>(dlsym(m_gioHandle, "g_settings_new"));
    const char* err = dlerror();
    if (err) {
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper,
            std::string("GnomeSetter: dlsym(g_settings_new) failed: ") + err);
        return false;
    }

    g_settings_set_string =
        reinterpret_cast<GSettingsSetString>(dlsym(m_gioHandle, "g_settings_set_string"));
    err = dlerror();
    if (err) {
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper,
            std::string("GnomeSetter: dlsym(g_settings_set_string) failed: ") + err);
        return false;
    }

    g_settings_sync = reinterpret_cast<GSettingsSync>(dlsym(m_gioHandle, "g_settings_sync"));
    err = dlerror();
    if (err) {
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper,
            std::string("GnomeSetter: dlsym(g_settings_sync) failed: ") + err);
        return false;
    }

    g_object_unref = reinterpret_cast<GObjectUnref>(dlsym(m_gioHandle, "g_object_unref"));
    err = dlerror();
    if (err) {
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper,
            std::string("GnomeSetter: dlsym(g_object_unref) failed: ") + err);
        return false;
    }

    return true;
}

void GnomeSetter::unloadGio() {
    if (m_gioHandle) {
        dlclose(m_gioHandle);
        m_gioHandle = nullptr;
    }
    g_settings_new = nullptr;
    g_settings_set_string = nullptr;
    g_settings_sync = nullptr;
    g_object_unref = nullptr;
    m_gioLoaded = false;
}

bool GnomeSetter::setWallpaper(const fs::path& path) {
    if (!m_gioLoaded) {
        m_logs.writeLogs(
            lvl::Error, mod::Wallpaper, "GnomeSetter: GIO not available, cannot set wallpaper");
        return false;
    }

    std::string pathStr = path.string();
    std::string uri = "file://" + pathStr;

    void* settings = g_settings_new("org.gnome.desktop.background");
    if (!settings) {
        m_logs.writeLogs(lvl::Error, mod::Wallpaper, "GnomeSetter: g_settings_new returned null");
        return false;
    }

    g_settings_set_string(settings, "picture-uri", uri.c_str());
    g_settings_set_string(settings, "picture-uri-dark", uri.c_str());
    g_settings_sync();
    g_object_unref(settings);

    m_logs.writeLogs(lvl::Info, mod::Wallpaper, "GnomeSetter: wallpaper set to " + pathStr);
    return true;
}
