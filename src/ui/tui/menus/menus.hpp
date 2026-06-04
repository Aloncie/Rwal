#pragma once
#include "keywords/keywords.hpp"
#include "settings/settings.hpp"
#include "settings/ISystemScheduler.hpp"
#include "settings/IConfigReader.hpp"
#include "ui/IUserInterface.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include "IWallpaperSetter.hpp"
#include "net/NetworkManager.hpp"
#include "internal/filesystem/IFileSystem.hpp"

#include <string>
#include <vector>

struct MenuResponse {
    std::string nextMenu;
	std::string Message = "";
    bool IsWrongInput = false;
    bool needQuit = false;
	bool needRefreshWallpaper = false;
};

class Menu {
public:
    virtual ~Menu() = default;
    virtual std::vector<std::string> getLines() = 0;
    virtual MenuResponse handleInput(const std::string& input) = 0;
    virtual const std::string& getValidChoices() const = 0;
};

class MainMenu : public Menu {
private:
    IUserInterface& m_uim;
    Keywords& m_keywords;
    WallpaperManager& m_wm;
	IWallpaperSetter& m_env;
	NetworkManager& m_netmanager;
    inline static const std::string m_validChoices = "1234q";

public:
    MainMenu(IUserInterface& uim, Keywords& keywords, WallpaperManager& wm, IWallpaperSetter& env, NetworkManager& netmanager);
    std::vector<std::string> getLines() override;
    MenuResponse handleInput(const std::string& input) override;
    const std::string& getValidChoices() const override { return m_validChoices; }
};

class SettingsMenu : public Menu {
private:
    ISystemScheduler& m_scheduler;
    WallpaperManager& m_wm;
	IUserInterface& m_uim;
	IFileSystem& m_fs;
    inline static const std::string m_validChoices = "12q";

public:
    SettingsMenu(ISystemScheduler& scheduler, WallpaperManager& wm, IUserInterface& ui, IFileSystem& fs);
    std::vector<std::string> getLines() override;
    MenuResponse handleInput(const std::string& input) override;
    const std::string& getValidChoices() const override { return m_validChoices; }
};

class KeywordsMenu : public Menu {
private:
    Keywords& m_keywords;
    IUserInterface& m_uim;
	IConfigReader& m_config;
    inline static const std::string m_validChoices = "armq";

public:
    KeywordsMenu(Keywords& keywords, IUserInterface& ui, IConfigReader& config);
    std::vector<std::string> getLines() override;
    MenuResponse handleInput(const std::string& input) override;
    const std::string& getValidChoices() const override { return m_validChoices; }
};

class SchedulerMenu : public Menu {
private:
    ISystemScheduler& m_scheduler;
    inline static const std::string m_validChoices = "nhd";

public:
    SchedulerMenu(ISystemScheduler& scheduler);
    std::vector<std::string> getLines() override;
    MenuResponse handleInput(const std::string& input) override;
    const std::string& getValidChoices() const override { return m_validChoices; }
};

