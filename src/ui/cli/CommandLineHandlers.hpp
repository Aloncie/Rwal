#pragma once
#include "settings/config.hpp"
#include "internal/filesystem/IFileSystem.hpp"
#include "net/NetworkManager.hpp"
#include "logs/logs.hpp"
#include "CommandLineParser.hpp"

namespace rwal::cli {
	struct CLIDependencies{
		IFileSystem& fs;
		Config& config;
		NetworkManager& nm;
		Logs& logs;
	};

	/**
     * @brief Main CLI entry point.
     * Always use this function. Do not call handlers directly.
     */

	int execute(const Options& options, const CLIDependencies& deps);

	// internal methods for handling commands, used by execute()
	namespace handlers{
		int handleVersion();
		int handleContact();
		int handleLogs(const Logs& logs);
		int handleChange(Logs& logs, Config& config, IFileSystem& fs, NetworkManager& nm);
		int handleSave(Logs& logs, Config& config, IFileSystem& fs);
		int handleClearLogs(Logs& logs);
		int handleKeywords(const Config& config);
		int handleSetKeywords(Config& config, const std::string& keywords);
		int handleRemoveKeywords(Config& config, const std::string& keywords);
		int handleAddKeywords(Config& config, const std::string& keywords);
	}
}
