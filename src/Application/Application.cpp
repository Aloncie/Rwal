#include "Application.hpp"

#include "AppConfig.h"
#include "internal/filesystem/FileSystemFactory.hpp"
#include "internal/utils/string_utils.hpp"
#include "keywords/keywords.hpp"
#include "logs/logs.hpp"
#include "net/CurlWrapper.hpp"
#include "net/NetworkManager.hpp"
#include "settings/ISystemScheduler.hpp"
#include "settings/SchedulerFactory.hpp"
#include "settings/config.hpp"
#include "settings/settings.hpp"
#include "wallpaper/IWallpaperSetter.hpp"
#include "wallpaper/WallpaperFactory.hpp"
#include "wallpaper/WallpaperManager.hpp"

#if RWAL_USE_TUI
#include "AppController/AppController.hpp"
#include "navigator/navigator.hpp"
#include "ui/tui/TUIManager.hpp"
#include "ui/tui/menus/menus.hpp"
#endif

#if RWAL_USE_CLI
#include "ui/cli/CLI.hpp"
#endif

#include <iostream>
#include <memory>
#include <thread>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

int Application::run(int argc, char* argv[]) {
    std::unique_ptr<IFileSystem> fs = createPlatformFileSystem();
    Logs logs(*fs);
    Config config(logs, *fs);
    CurlWrapper curl(logs, *fs);
    NetworkManager netmanager(curl, config, logs, *fs);
    std::unique_ptr<ISystemScheduler> scheduler = createPlatformScheduler(logs, *fs);

#if RWAL_USE_CLI
    bool hasCliOptions = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.size() > 1 && (arg[0] == '-')) {
            hasCliOptions = true;
            break;
        }
    }

    if (hasCliOptions) {
        CLI cli(*fs, config, netmanager, logs, *scheduler);
        cli.parse(argc, argv);
        return cli.execute();
    }
#endif
// two different if-endif because file can be TUI and CLI in the one binary
#if RWAL_USE_TUI
    Keywords keywords(config, logs, *fs);
    std::unique_ptr<IWallpaperSetter> env = createWallpaperSetter(logs);
    WallpaperManager wm(logs, *fs);

    if (argc > 1) {
        std::cerr << "Error: Rwal TUI mode doesn't support flags yet. \nPlease run without "
                     "arguments to start TUI."
                  << std::endl;
        return 1;
    }
    TUIManager tuim;
    std::unique_ptr<ISystemScheduler> schedule = createPlatformScheduler(logs, *fs);

    tuim.initUI();

    auto mainMenu = std::make_unique<MainMenu>(tuim, keywords, wm, *env, netmanager);
    auto settingsMenu = std::make_unique<SettingsMenu>(*schedule, wm, tuim, *fs);
    auto keywordsMenu = std::make_unique<KeywordsMenu>(keywords, tuim, config);
    auto scheduleMenu = std::make_unique<SchedulerMenu>(*schedule);

    Navigator navigator(logs);
    navigator.registerMenu("main", std::move(mainMenu));
    navigator.registerMenu("settings", std::move(settingsMenu));
    navigator.registerMenu("keywords", std::move(keywordsMenu));
    navigator.registerMenu("scheduler", std::move(scheduleMenu));

    navigator.start("main");

    logs.writeLogs(lvl::Info, mod::Core, "Rwal's start in normal mode");

    std::jthread wallpaperThread;
    std::atomic<bool> refreshDone(false);
    std::string refreshError;

    AppController controller(
        navigator, tuim, wm, *env, netmanager, keywords, wallpaperThread, refreshDone,
        refreshError);

    while (controller.handleStdin()) {
        controller.checkRefreshDone();
        // small sleep to avoid CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (wallpaperThread.joinable()) {
        wallpaperThread.join();
    }

    tuim.shutdownUI();
    return 0;
#endif
    std::cout << R"(Failed to start Rwal, this binary doesn't touch TUI and CLI.
		Try to use these flags: 
		For TUI support -DRWAL_USE_TUI=ON 
		For CLI support -DRWAL_USE_CLI=ON 
		You can also use both flags.)"
              << std::endl;
    return 1;
}
