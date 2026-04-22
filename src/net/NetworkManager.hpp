#pragma once
#include "CurlWrapper.hpp"
#include "settings/IConfigReader.hpp"
#include "logs/logs.hpp"

#include <optional>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string_view>

namespace fs = std::filesystem;

class NetworkManager{
public:
	NetworkManager(CurlWrapper& curl, IConfigReader& config, Logs& logs) : m_curl(curl), m_config(config),m_logs(logs) {}
	virtual std::optional<fs::path> fetchImage(std::string_view keyword);
	virtual bool isAvailable();
	virtual std::string craftUrl(std::string_view keyword, const std::optional<std::string>& page = std::nullopt);
	virtual ~NetworkManager() = default;
private:
	Logs& m_logs;
	CurlWrapper& m_curl;
	IConfigReader& m_config;
};

