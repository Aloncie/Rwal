#pragma once
#include "CurlWrapper.hpp"
#include "settings/config.hpp"
#include <optional>
#include <string>

class NetworkManager{
public:
	NetworkManager(MyCurl& curl, Config& config);
	std::string fetchImage(std::string keyword);
	bool isAvailable();
	std::string craftUrl(std::string keyword, std::optional<std::string> page = std::nullopt);
	~NetworkManager() = default;
private:
	MyCurl& m_curl;
	Config& m_config;
};
