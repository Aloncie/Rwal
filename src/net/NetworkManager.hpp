#pragma once
#include "CurlWrapper.hpp"
#include "settings/IConfigReader.hpp"
#include "logs/logs.hpp"

#include <optional>
#include <string>
#include <filesystem>
#include <functional>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

class NetworkManager{
public:
	NetworkManager(CurlWrapper& curl, IConfigReader& config, Logs& logs) : m_curl(curl), m_config(config),m_logs(logs) {}
	std::optional<fs::path> fetchImage(std::string keyword);
	bool isAvailable();
	std::string craftUrl(std::string keyword, std::optional<std::string> page = std::nullopt);
	~NetworkManager() = default;
private:
	Logs& m_logs;
	CurlWrapper& m_curl;
	IConfigReader& m_config;
};

