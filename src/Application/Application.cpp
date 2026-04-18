#include "Application.hpp"
#include "keywords/keywords.hpp"
#include "logs/logs.hpp"
#include "net/CurlWrapper.hpp"
#include "net/NetworkManager.hpp"
#include "settings/config.hpp"
#include "settings/settings.hpp"
#include "wallpaper/IWallpaperSetter.hpp"
#include "wallpaper/WallpaperFactory.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include "internal/filesystem/FileSystemFactory.hpp"
#include "AppConfig.h"
#include "internal/utils/string_utils.hpp"

#if RWAL_USE_TUI
	#include "ui/tui/TUIManager.hpp"
	#include "navigator/navigator.hpp"
	#include "AppController/AppController.hpp"
	#include "ui/tui/menus/menus.hpp"
#endif

#if RWAL_USE_CLI
	#include "ui/cli/CommandLineParser.hpp"
	#include "ui/cli/CommandLineHandlers.hpp"
#endif

#include <QCoreApplication>
#include <QObject>
#include <QCommandLineParser>
#include <QApplication>
#include <memory>

int Application::run(int argc, char* argv[]) {
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
	QCoreApplication app(argc, argv);

    Logs logs;
	Config config(logs);
    std::unique_ptr<IFileSystem> fs = createPlatformFileSystem();
    Keywords keywords(config, logs);
    CurlWrapper curl(logs);
    NetworkManager nm(curl, config, logs);
    WallpaperFactory wf(logs);
    std::unique_ptr<IWallpaperSetter> env = wf.create();
    WallpaperManager wm(logs, *fs);

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
		rwal::cli::CLIDependencies deps(*fs, config, nm, logs);
		rwal::cli::Options opts = rwal::cli::parse(app);
		return rwal::cli::execute(opts, deps);
	}
#endif

#if RWAL_USE_TUI
	if (argc > 1) { 
		std::cerr << "Error: Rwal TUI mode doesn't support flags yet. \nPlease run without arguments to start TUI." << std::endl;
		return 1;
	}
    TUIManager tuim;
	Timer timer(logs);

    tuim.initUI();

    auto mainMenu = std::make_unique<MainMenu>(tuim, keywords, wm, *env, nm);
    auto settingsMenu = std::make_unique<SettingsMenu>(timer, wm, tuim);
    auto keywordsMenu = std::make_unique<KeywordsMenu>(keywords, tuim, config);
    auto timerMenu = std::make_unique<TimerMenu>(timer);

    Navigator navigator(logs);
    navigator.registerMenu("main", std::move(mainMenu));
    navigator.registerMenu("settings", std::move(settingsMenu));
    navigator.registerMenu("keywords", std::move(keywordsMenu));
    navigator.registerMenu("timer", std::move(timerMenu));

    navigator.start("main");

    AppController controller(navigator, tuim);
    logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Core, "Rwal's start in normal mode");

    int one = app.exec();
    tuim.shutdownUI();
    return one;
#endif
	std::cout << R"(Failed to start Rwal, this binary doesn't touch TUI and CLI.
		Try to use these flags: 
		For TUI support -DRWAL_USE_TUI=ON 
		For CLI support -DRWAL_USE_CLI=ON 
		You can also use both flags.)" << std::endl;
	return 1;
}

