#include "CommandLineParser.hpp"

#include <QCommandLineParser>
#include <QString>

namespace rwal::cli {
	Options parse(QCoreApplication& app){
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

		Options options;
		options.showHelp = parser.isSet("help");
		options.showVersion = parser.isSet(versionOption);
		options.changeWallpaper = parser.isSet(changeOption);
		options.saveWallpaper = parser.isSet(saveOption);
		options.showContact = parser.isSet(contactOption);
		options.showLogs = parser.isSet(logOption);
		options.clearLogs = parser.isSet(clearLogsOption);
		options.showKeywords = parser.isSet(keywordsOption);
		
		if (parser.isSet(setKeywordsOption)) {
            options.setKeywords = parser.value(setKeywordsOption).toStdString();
        } else if (parser.isSet(addKeywordsOption)) {
			options.addKeywords = parser.value(addKeywordsOption).toStdString();
		} else if (parser.isSet(removeKeywordsOption)) {
			options.removeKeywords = parser.value(removeKeywordsOption).toStdString();
		}

		return options;
	}
}
