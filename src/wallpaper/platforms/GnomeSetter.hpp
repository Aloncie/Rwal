#pragma once
#include "logs/logs.hpp"
#include "wallpaper/IWallpaperSetter.hpp"

class GnomeSetter : public IWallpaperSetter {
public:
    bool setWallpaper(const fs::path& path) override;
    explicit GnomeSetter(Logs& logs);
    ~GnomeSetter() override;
    bool isAvailable() const override { return m_gioLoaded; }

private:
    // --- GIO dynamic loading ---
    bool loadGio();
    void unloadGio();

    void* m_gioHandle = nullptr;

    // Function pointer types
    using GSettingsNew = void* (*)(const char*);
    using GSettingsSetString = int (*)(void*, const char*, const char*);
    using GSettingsSync = void (*)();
    using GObjectUnref = void (*)(void*);

    GSettingsNew g_settings_new = nullptr;
    GSettingsSetString g_settings_set_string = nullptr;
    GSettingsSync g_settings_sync = nullptr;
    GObjectUnref g_object_unref = nullptr;

    bool m_gioLoaded = false;
    Logs& m_logs;
};
