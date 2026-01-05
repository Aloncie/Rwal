#pragma once
#include <string>
#include <type_traits>
#include <vector>
#include <nlohmann/json.hpp>
#include <settings/config.h>

extern const std::string keywords_path;

class Keywords{
private:
	std::vector<std::string> LongWayGetKeywords();
public:
	void Default(std::vector<std::string>& Keywords);

	std::string LongGetRandomKeywords(const std::string& mode);
	void open_keywords_editor();
	void Format(std::string& str);

	std::string GetRandomKeywords(const std::string& mode);

	template<typename T = std::vector<std::string>>
	T ShortWayGetKeywords(){
		auto search = Config::getInstance().get<nlohmann::json>("search");
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
			static_assert(false, "Unsupported type");
		}
	};


};

