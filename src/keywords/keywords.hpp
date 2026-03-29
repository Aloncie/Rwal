#pragma once
#include <string>
#include <type_traits>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "ui/cli/UIManager.hpp"
#include <type_traits>
#include "settings/IConfigReader.hpp"

namespace fs = std::filesystem;

extern const std::string keywords_path;

class Keywords{
private:
	void promptForKeywords(std::function<void(std::vector<std::string>)> callback);
	void importToTxt(fs::path& path);
	std::vector<std::string> exportFromTxt(fs::path& path);
	UIManager& m_ui;
	IConfigReader& m_config;

	template<typename> struct always_false : std::false_type {};
public:
	Keywords(UIManager& ui,IConfigReader& config);

	void LongWayGetKeywords(std::function<void(std::vector<std::string>)> callback);
	void Default(std::vector<std::string>& Keywords);
	void editKeywords();
	void GetRandomKeywords(std::function<void(std::string)> callback, const std::string& mode);

	template<typename T = std::vector<std::string>>
	T ShortWayGetKeywords(){
		auto search = m_config.get<nlohmann::json>("search");
		std::vector<std::string> keywords = search["keywords"].get<std::vector<std::string>>();

		if constexpr(std::is_same_v<T,std::string>){

			std::string result;
			for (const auto& kw : keywords) {
				if (!result.empty()) result += ", ";
				result += kw;
			}
			return result;
		} else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
			return keywords;
		} else {
			static_assert(always_false<T>::value, "Unsupported type");
			return T{}; 
		}
	};
	
};

