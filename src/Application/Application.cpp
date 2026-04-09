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
#include "internal/filesystem/FileSystemFactory.hpp"
#include "AppConfig.h"

#include <QCoreApplication>
#include <QObject>
#include <QCommandLineParser>
#include <memory>
#include <iostream>

int Application::run(int argc, char* argv[]) {
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Rwal - cross-platform wallpaper utility");
    parser.addHelpOption();

    QCommandLineOption changeOption({"c", "change"}, "Change wallpaper one and exit");
    QCommandLineOption saveOption({"s", "save"}, "Save current wallpaper");
	QCommandLineOption contactOption("contact", "Show contact information");
	QCommandLineOption versionOption({"v","version"}, "Show application version");

    parser.addOption(changeOption);
    parser.addOption(saveOption);
	parser.addOption(contactOption);
	parser.addOption(versionOption);
    parser.process(app);

    Logs logs;
    auto fs = createPlatformFileSystem();
    if (!fs) {
        logs.writeLogs("Failed to initialize file system");
        return 1;
    }
	
	if (parser.isSet(versionOption)) {
		std::cout << APP_NAME << " " << APP_VERSION << std::endl;
		std::cout << COPYRIGHT_STR << std::endl;
		return 0;
	}
    if (parser.isSet(changeOption)) {
        UIManager uim;
        Config config(logs);
        Keywords keywords(config, logs);
        std::unique_ptr curl = std::make_unique<CurlWrapper>(logs);
        NetworkManager nm(*curl, config, logs);
        WallpaperFactory wf(logs);
        std::unique_ptr<IWallpaperSetter> env = wf.create();
        WallpaperManager wm(logs, *fs);
        logs.writeLogs("Rwal's start in change mode");

        wm.refresh(*env, nm, keywords, nullptr, "change");
        return 0;
    } else if (parser.isSet(saveOption)) {
        UIManager uim;
        Config config(logs);
        CurlWrapper curl(logs);
        NetworkManager nm(curl, config, logs);
        WallpaperFactory wf(logs);
        std::unique_ptr<IWallpaperSetter> env = wf.create();
        WallpaperManager wm(logs, *fs);
        std::string message = wm.saveCurrent();

        logs.writeLogs("Rwal's start for save current wallpaper");
        logs.writeLogs(message);
        return 0;
    } else if (parser.isSet(contactOption)) {
        std::string contactInfo =
            "Contact Information:\n"
            "Email: " +
            std::string(APP_EMAIL) +
            "\n"
            "Repository: " +
            std::string(APP_REPOSITORY_URL) +
            "\n"
            "Issues: " +
            std::string(APP_ISSUES_URL) +
			"\n"
			"\n"
			"Copyright: " +
			std::string(COPYRIGHT_STR);
        std::cout << contactInfo << std::endl;
        return 0;
    }

    UIManager uim;
    Config config(logs);
    Keywords keywords(config, logs);
    CurlWrapper curl(logs);
    Timer timer(logs);
    NetworkManager nm(curl, config, logs);
    WallpaperFactory wf(logs);
    std::unique_ptr<IWallpaperSetter> env = wf.create();
    WallpaperManager wm(logs, *fs);

    uim.initUI();

    auto mainMenu = std::make_unique<MainMenu>(uim, keywords, wm, *env, nm);
    auto settingsMenu = std::make_unique<SettingsMenu>(timer, wm, uim);
    auto keywordsMenu = std::make_unique<KeywordsMenu>(keywords, uim, config);
    auto timerMenu = std::make_unique<TimerMenu>(timer);

    Navigator navigator(logs);
    navigator.registerMenu("main", std::move(mainMenu));
    navigator.registerMenu("settings", std::move(settingsMenu));
    navigator.registerMenu("keywords", std::move(keywordsMenu));
    navigator.registerMenu("timer", std::move(timerMenu));

    navigator.start("main");

    AppController controller(&navigator, uim);
    logs.writeLogs("Rwal's start in normal mode");

    int one = app.exec();
    uim.shutdownUI();
    return one;
}

