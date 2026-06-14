#pragma once
#include "navigator/navigator.hpp"
#include "ui/tui/menus/menus.hpp"
#include "ui/tui/TUIManager.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include "wallpaper/IWallpaperSetter.hpp"
#include "net/NetworkManager.hpp"
#include "keywords/keywords.hpp"

#include <thread>
#include <atomic>

class AppController {
public:
    AppController(
		Navigator& nav, 
		TUIManager& tui, 
		WallpaperManager& wmanager, 
		IWallpaperSetter& env, 
		NetworkManager& netmanager, 
		Keywords& keywords, 
		std::jthread& wallpaperThread, 
		std::atomic<bool>& refreshDone, 
		std::string& refreshError
	);

	~AppController() = default;

	bool handleStdin();
	void checkRefreshDone();
private:
    Navigator& m_navigator;
	TUIManager& m_tui;
	WallpaperManager& m_wmanager;
	IWallpaperSetter& m_env;
	NetworkManager& m_netmanager;
	Keywords& m_keywords;
	std::jthread& m_wallpaperThread;
	std::atomic<bool>& m_refreshDone;
	std::string& m_refreshError;

	void launchRefreshWallpaper();
};

