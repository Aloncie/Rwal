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
	QCommandLineOption saveOption({"s", "save"}, "Save current wallpaper");

    parser.addOption(changeOption);
	parser.addOption(saveOption);
    parser.process(app);

    if (parser.isSet(changeOption)) {
        UIManager uim;
        Config config;
        Keywords keywords(config);
        CurlWrapper curl;
        NetworkManager nm(curl, config);
        WallpaperFactory wf;
        std::unique_ptr<IWallpaperSetter> env = wf.create();
        WallpaperManager wm;
		std::string message = wm.refresh(*env, nm, keywords, nullptr, "change");
        Logs::init(uim);
        Logs::getInstance().writeLogs("Rwal's start in change mode");
        Logs::getInstance().writeLogs(message);
        return 0;
    }
	else if (parser.isSet(saveOption)) {
		UIManager uim;
		Config config;
		CurlWrapper curl;
		NetworkManager nm(curl, config);
		WallpaperFactory wf;
		std::unique_ptr<IWallpaperSetter> env = wf.create();
		WallpaperManager wm;
		std::string message = wm.saveCurrent();
		Logs::init(uim);
		Logs::getInstance().writeLogs("Rwal's start for save current wallpaper");
		Logs::getInstance().writeLogs(message);
		return 0;
	}

    UIManager uim;
    Config config;
    Keywords keywords(config);
    CurlWrapper curl;
    Timer timer;
    NetworkManager nm(curl, config);
    WallpaperFactory wf;
    std::unique_ptr<IWallpaperSetter> env = wf.create();
    WallpaperManager wm;

    uim.initUI();

    auto mainMenu = std::make_unique<MainMenu>(uim, keywords, wm, *env, nm);
    auto settingsMenu = std::make_unique<SettingsMenu>(timer, wm, uim);
    auto keywordsMenu = std::make_unique<KeywordsMenu>(keywords, uim, config);
    auto timerMenu = std::make_unique<TimerMenu>(timer);

    Navigator navigator;
    navigator.registerMenu("main", std::move(mainMenu));
    navigator.registerMenu("settings", std::move(settingsMenu));
    navigator.registerMenu("keywords", std::move(keywordsMenu));
    navigator.registerMenu("timer", std::move(timerMenu));

    navigator.start("main");

    AppController controller(&navigator, uim);
    Logs::init(uim);
    Logs::getInstance().writeLogs("Rwal's start in normal mode");

    int one = app.exec();
    uim.shutdownUI();
    return one;
}

