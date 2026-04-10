#pragma once

#include <QString>
#include <optional>
#include <string>

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
		bool listKeywords = false;
		
		std::optional<std::string> setKeywords;
		std::optional<std::string> addKeywords;
		std::optional<std::string> removeKeywords;
	};	
	QString parse(int argc, char** argv[]);
}
