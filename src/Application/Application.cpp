#include "Application.hpp"
#include "AppController/AppController.hpp"
#include "keywords/keywords.hpp"
#include "logs/logs.hpp"
#include "navigator/navigator.hpp"
#include "net/CurlWrapper.hpp"
#include "net/NetworkManager.hpp"
#include "settings/config.hpp"
#include "settings/settings.hpp"
#include "ui/tui/TUIManager.hpp"
#include "ui/menus/menus.hpp"
#include "wallpaper/IWallpaperSetter.hpp"
#include "wallpaper/WallpaperFactory.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include "internal/filesystem/FileSystemFactory.hpp"
#include "AppConfig.h"
#include "internal/utils/string_utils.hpp"

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
	QCommandLineOption logOption("log", "Show latest logs");
	QCommandLineOption clearLogsOption("clear-logs", "Clear logs file");
	QCommandLineOption keywordsOption({"k","keywords"}, "Show keywords");
	QCommandLineOption setKeywordsOption("set-keywords", "Set keywords for wallpaper search (comma-separated, e.g.: nature,ocean,town)", "keywords");
	QCommandLineOption addKeywordsOption("add-keywords", "Add keywords for wallpaper search (comma-separated, e.g.: nature,ocean,town)", "keywords");
	QCommandLineOption removeKeywordsOption("remove-keywords", "Remove keywords for wallpaper search (comma-separated, e.g.: nature,ocean,town)", "keywords");

    parser.addOption(changeOption);
    parser.addOption(saveOption);
	parser.addOption(contactOption);
	parser.addOption(versionOption);
	parser.addOption(logOption);
	parser.addOption(clearLogsOption);
	parser.addOption(keywordsOption);
	parser.addOption(setKeywordsOption);
	parser.addOption(addKeywordsOption);
	parser.addOption(removeKeywordsOption);

    parser.process(app);

    Logs logs;
	Config config(logs);
    auto fs = createPlatformFileSystem();
    if (!fs) {
        logs.writeLogs("Failed to initialize file system");
        return 1;
    }
	
	if (parser.isSet(versionOption)) {
		std::cout << APP_NAME << " " << APP_VERSION << std::endl;
		std::cout << COPYRIGHT_STR << std::endl;
		return 0;
	} else if (parser.isSet(changeOption)) {
        TUIManager uim;
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
        TUIManager uim;
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
    } else if (parser.isSet(logOption)) {
    		std::string logsContent = logs.getLogs();
    		std::cout << logsContent << std::endl;
    		return 0;
	} else if (parser.isSet(clearLogsOption)) {
		if (logs.refresh()) {
			std::cout << "Logs cleared successfully." << std::endl;
		} else {
			std::cout << "Failed to clear logs." << std::endl;
		}
		return 0;
	} else if (parser.isSet(keywordsOption)) {
		std::vector<std::string> keywords = config.getImpl("/search/keywords");
		if (keywords.empty()) {
			std::cout << "There are no keywords in your config file." << std::endl;
		}
		std::cout << "Keywords: ";
		for (int i = 0; i < keywords.size(); i++) {
			std::cout << keywords[i];
			if (i < keywords.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << std::endl;
		return 0;
	} else if (parser.isSet(setKeywordsOption)) {
		std::string option = parser.value(setKeywordsOption).toStdString();
		if (option.empty()) {
			std::cout << "No keywords provided. Please provide comma-separated keywords." << std::endl;
			return 0;
		}

		rwal::utils::string::format(option);
		std::vector<std::string> keywords = rwal::utils::string::split_by_space(option);

		config.setImpl("/search/keywords", keywords);
		std::cout << "New keywords have been set successfully: " << option << std::endl;
		return 0;
	} else if (parser.isSet(addKeywordsOption)) {
        std::string option = parser.value(addKeywordsOption).toStdString();
        if (option.empty()) {
            std::cout << "No keywords provided. Please provide comma-separated keywords." << std::endl;
            return 0;
        }
		
		rwal::utils::string::format(option);
		std::vector<std::string> oldKeywords = config.getImpl("/search/keywords");
		std::vector<std::string> newKeywords = rwal::utils::string::split_by_space(option);

		oldKeywords.insert(oldKeywords.end(), newKeywords.begin(), newKeywords.end());

		config.setImpl("/search/keywords", oldKeywords);
        std::cout << "Keywords have been added successfully: " << option << std::endl;
        return 0;
	} else if (parser.isSet(removeKeywordsOption)) {
		std::string option = parser.value(removeKeywordsOption).toStdString();
		std::vector<std::string> keywords = config.getImpl("/search/keywords");
		rwal::utils::string::format(option);
		std::vector<std::string> needRemove = rwal::utils::string::split_by_space(option);
		std::string removedStr = "Removed keywords: ";
		for (int i = 0; i < needRemove.size(); i++) {
            for (int j = 0; j < keywords.size(); j++) {
                if (keywords[j] == needRemove[i]) {
					removedStr += keywords[j] + " ";
                    keywords.erase(keywords.begin() + j);
                    j--;
                }
            }
        }
		std::cout << removedStr << std::endl;
        config.setImpl("/search/keywords", keywords);
        return 0;
	}


    TUIManager uim;
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

