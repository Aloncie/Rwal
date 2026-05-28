#pragma once
#include "settings/config.hpp"
#include "internal/filesystem/IFileSystem.hpp"
#include "net/NetworkManager.hpp"
#include "logs/logs.hpp"

#include <optional>
#include <string>

class CLI {
private:
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
	
	Options m_opts;

	struct Dependencies {
		IFileSystem& fs;
		Config& config;
		NetworkManager& netmanager;
		Logs& logs;
	};
	
	Dependencies m_deps;

	// Handlers
	int handleHelp();
	int handleVersion();
	int handleContact();
	int handleLogs();
	int handleChange();
	int handleSave();
	int handleClearLogs();
	int handleKeywords();
	int handleSetKeywords();
	int handleRemoveKeywords();
	int handleAddKeywords();
	
	// Helper functions
	std::string getBinaryLocation();
public:
	CLI(IFileSystem& fs, Config& config, NetworkManager& nm, Logs& logs) : m_deps{fs, config, nm, logs} {}
	~CLI() = default;
	void parse(int argc, char* argv[]);
	int execute();
};

