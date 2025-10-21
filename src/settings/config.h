#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace fs = std::filesystem;

class Config{
private:
	nlohmann::json data;
	fs::path configPath;
	fs::path getConfigPath();
	void saveConfig();
public:
	Config();

	template<typename G>
	G get(const std::string& key);

	template<typename S>
	void set(const std::string& key, const S& value);
};
