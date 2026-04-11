#pragma once

#include <optional>
#include <string>
#include <QCoreApplication>

namespace rwal::cli{
	struct Options {
		bool showHelp = false;
		bool showVersion = false;
		bool changeWallpaper = false;
		bool saveWallpaper = false;
		bool showContact = false;
		bool showLogs = false;
		bool clearLogs = false;
		bool showKeywords = false;
		
		std::optional<std::string> setKeywords;
		std::optional<std::string> addKeywords;
		std::optional<std::string> removeKeywords;
	};	
	Options parse(QCoreApplication& app);
}
