#include "Application.hpp"
#include "AppController/AppController.hpp"
#include "keywords/keywords.hpp"
#include "logs/logs.hpp"
#include "navigator/navigator.hpp"
#include "net/CurlWrapper.hpp"
#include "net/NetworkManager.hpp"
#include "settings/config.hpp"
#include "settings/settings.hpp"
#include "ui/cli/UIManager.hpp"
#include "ui/menus/menus.hpp"
#include "wallpaper/IWallpaperSetter.hpp"
#include "wallpaper/WallpaperFactory.hpp"
#include "wallpaper/WallpaperManager.hpp"

#include <QCoreApplication>
#include <QObject>

int Application::run(int argc, char* argv[]) {
    QCoreApplication::setApplicationName("Rwal");
    QCoreApplication::setOrganizationName("Aloncie");
    QCoreApplication app(argc, argv);

    UIManager um;
    Config config;
    Keywords keywords(um, config);
    CurlWrapper curl;
    Timer timer;
    NetworkManager nm(curl, config);
    WallpaperFactory wf;
    std::unique_ptr<IWallpaperSetter> env = wf.create();
    WallpaperManager wm(um, keywords, nm, *env);

    if (argc > 1 && (strcmp(argv[1], "--change") == 0 || strcmp(argv[1], "-c") == 0)) {
        wm.refresh("change");
        return 0;
    }

    um.initUI();

    auto mainMenu = std::make_unique<MainMenu>(um, keywords, wm);
    auto settingsMenu = std::make_unique<SettingsMenu>(timer, wm);
    auto keywordsMenu = std::make_unique<KeywordsMenu>(keywords, um, config);
    auto timerMenu = std::make_unique<TimerMenu>(timer);

    Navigator navigator;
    navigator.registerMenu("main", std::move(mainMenu));
    navigator.registerMenu("settings", std::move(settingsMenu));
    navigator.registerMenu("keywords", std::move(keywordsMenu));
    navigator.registerMenu("timer", std::move(timerMenu));

    navigator.start("main");

    AppController controller(&navigator, um);
    Logs::init(um);
    Logs::getInstance().writeLogs("Rwal's start");

    int one = app.exec();
    um.shutdownUI();
    return one;
}

