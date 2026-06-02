#include "config.hpp"
#include "AppConfig.h"

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

fs::path Config::getConfigPath(){
	configPath = m_fs.getConfigLocation() / ORGANIZATION_NAME / APP_NAME / "config.json";

	if (!m_fs.exists(configPath.parent_path())) {
		m_logs.writeLogs(lvl::Info, mod::Config, "Config directory not found, creating it");
		m_fs.createDirectories(configPath.parent_path());
	}

	return configPath;
}

Config::Config(Logs& logs, IFileSystem& fs) : IConfigReader(logs), m_fs(fs) {
    configPath = getConfigPath();
    initValidators();
	getConfigFileData();
}

void Config::getConfigFileData(){
	if (m_fs.exists(configPath)) {
        std::ifstream file(configPath);
        if (file.is_open()) {
            try {
                m_data = nlohmann::json::parse(file);
                m_logs.writeLogs(lvl::Info, mod::Config, "Config loaded/reloaded: " + configPath.string());
            } catch (nlohmann::json::parse_error& e) {
                m_logs.writeLogs(lvl::Error, mod::Config, "JSON Parse Error: " + std::string(e.what()));
            }
        }
    } else {	
		m_data = {
   			{"services", {
        		{"wallhaven", {
            		{"apikey", ""},
            		{"base_url", "https://wallhaven.cc/api/v1/search"},
            		{"param_names", {
                		{"query", "?q"},
						{"page", "page"},
						{"apikey", "apikey"},
                		{"sorting", "sorting"},
                		{"res", "resolutions"}
            	}}
        	}}
    		}},
    		{"search", {
        		{"keywords", {}},
        		{"sorting", "random"},
        		{"res", "1920x1080"},
				{"random_page", true}

    		}},
			{"settings", {
				// Planned implementation
				{"cursor-visibility", true}
			}}
		};
		saveToFile();
	};		
}

void Config::saveToFile(){
	// Use temporary file to avoid data loss
	auto tmp = getConfigPath().string() + ".tmp";
    std::ofstream file(tmp);
    file << m_data.dump(2);
    file.close();
    m_fs.rename(tmp, configPath);  // replacement
}

void Config::initValidators(){
	auto is_not_empty_string = [](const nlohmann::json& j){
		return j.is_string() && !j.get<std::string>().empty(); 
				};
	auto is_not_empty_array = [](const nlohmann::json& j){
		return j.is_array() && !j.empty();
	};
	auto is_not_empty_bool = [](const nlohmann::json& j){
        return j.is_boolean();
    };
	auto is_not_empty_object = [](const nlohmann::json& j){
        return (j.is_object() || j.is_array()) && !j.empty();
    };

	validators["/search/keywords"] = is_not_empty_array;
	validators["/services/wallhaven/apikey"] = is_not_empty_string;
	validators["/search/sorting"] = is_not_empty_string;
    validators["/search/res"] = is_not_empty_string;
    validators["/settings/cursor-visibility"] = is_not_empty_bool;
	validators["/search/random_page"] = is_not_empty_bool;
	validators["/services/wallhaven/base_url"] = is_not_empty_string;
	validators["/services/wallhaven/param_names/query"] = is_not_empty_string;
    validators["/services/wallhaven/param_names/sorting"] = is_not_empty_string;
    validators["/services/wallhaven/param_names/res"] = is_not_empty_string;
	validators["/services/wallhaven/param_names"] = is_not_empty_array;
	validators["/services/wallhaven"] = is_not_empty_object;
	validators["/search"] = is_not_empty_object;
	validators["/services"] = is_not_empty_object;
    validators["/"] = is_not_empty_object;
}

