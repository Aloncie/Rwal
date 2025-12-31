#pragma once
#include "CurlWrapper.h"
#include <optional>
#include <string>

class NetworkManager{
public:
	static NetworkManager& getInstance();
	
	std::string fetchImage(std::string keyword);
	bool isAvailable();
	std::string craftUrl(std::string keyword, std::optional<std::string> page = std::nullopt);
private:
	NetworkManager();
	~NetworkManager() = default;

	NetworkManager(const NetworkManager&) = delete;
	NetworkManager& operator=(const NetworkManager&) = delete;

	MyCurl mycurl;
};
