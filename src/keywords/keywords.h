#pragma once
#include <string>
#include <type_traits>
#include <vector>
#include <nlohmann/json.hpp>
#include <settings/config.h>

extern const std::string keywords_path;

class Keywords{
public:
	void Default(std::vector<std::string>& Keywords);

	template<typename T = std::vector<std::string>>
	T look_keywords(){
		Config c;
		auto search = c.get<nlohmann::json>("search");
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

	std::vector<std::string> get_keywords();
	void open_keywords_editor();
};

