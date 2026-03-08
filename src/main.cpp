#include "navigator/navigator.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include <string>
#include <QCoreApplication>
#include <QObject>
#include "ui/menus/menus.hpp"
#include "AppController/AppController.hpp"
#include <iostream>
#include "ui/cli/UIManager.hpp"
#include "keywords/keywords.hpp"
#include "logs/logs.hpp"
#include "settings/settings.hpp"
#include "settings/config.hpp"
#include "net/CurlWrapper.hpp"
#include "net/NetworkManager.hpp"

int main(int argc,char* argv[]){
    QCoreApplication::setApplicationName("Rwal");
    QCoreApplication::setOrganizationName("Aloncie");
    QCoreApplication app(argc, argv);

    UIManager um;
    Config config;
    Keywords keywords(um, config);
    MyCurl curl;
    Timer timer;
    NetworkManager nm(curl, config);
    WallpaperManager wm(um, keywords, nm);

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
    Logs::getInstance().writeLogs("Rwal started");

    int one = app.exec();
    um.shutdownUI();
    return one;
}
