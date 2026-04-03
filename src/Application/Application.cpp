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
#include <QCommandLineParser>

int Application::run(int argc, char* argv[]) {
    QCoreApplication::setApplicationName("Rwal");
    QCoreApplication::setOrganizationName("Aloncie");
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Rwal - cross-platform wallpaper utility");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption changeOption({"c", "change"}, "Change wallpaper one and exit");

    parser.addOption(changeOption);
    parser.process(app);

    if (parser.isSet(changeOption)) {
        UIManager um;
        Config config;
        Keywords keywords(config);
        CurlWrapper curl;
        NetworkManager nm(curl, config);
        WallpaperFactory wf;
        std::unique_ptr<IWallpaperSetter> env = wf.create();
        WallpaperManager wm(um, keywords, nm, *env);
        std::string message = wm.refresh();
        Logs::init(um);
        Logs::getInstance().writeLogs("Rwal's start in change mode");
        Logs::getInstance().writeLogs(message);
        return 0;
    }

    UIManager um;
    Config config;
    Keywords keywords(config);
    CurlWrapper curl;
    Timer timer;
    NetworkManager nm(curl, config);
    WallpaperFactory wf;
    std::unique_ptr<IWallpaperSetter> env = wf.create();
    WallpaperManager wm(um, keywords, nm, *env);

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
    Logs::getInstance().writeLogs("Rwal's start in normal mode");

    int one = app.exec();
    um.shutdownUI();
    return one;
}

