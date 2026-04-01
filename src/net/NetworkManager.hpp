#pragma once
#include "CurlWrapper.hpp"
#include "settings/IConfigReader.hpp"
#include <optional>
#include <string>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

class NetworkManager{
public:
	NetworkManager(CurlWrapper& curl, IConfigReader& config);
	std::optional<fs::path> fetchImage(std::string keyword);
	bool isAvailable();
	std::string craftUrl(std::string keyword, std::optional<std::string> page = std::nullopt);
	~NetworkManager() = default;
private:
	CurlWrapper& m_curl;
	IConfigReader& m_config;
};
