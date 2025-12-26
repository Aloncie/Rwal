#pragma once
#include "CurlWrapper.h"
#include <string>

class NetworkManager{
public:
	static NetworkManager& getInstance();

	bool isAvailable();

	std::string fetchWallpaperUrl(const std::string& query);
private:
	NetworkManager();
	~NetworkManager() = default;

	NetworkManager(const NetworkManager&) = delete;
	NetworkManager& operator=(const NetworkManager&) = delete;
};
