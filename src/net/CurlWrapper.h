#pragma once
#include <curl/curl.h>
#include <random>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>

namespace fs = std::filesystem;

static size_t callback (void* contents, size_t size, size_t nmemb, void* userp);

class MyCurl{
private:
	CURL* curl;
	long http_code = 0;
	std::string url = "https://wallhaven.cc/api/v1/search?q=";
	std::string page, buffer;
	nlohmann::json j;
public:

	MyCurl(std::string api_key, std::string keyword);
	void get_request();
	std::string get_image_url();
	std::string get_count_pages();
	std::string download_image(const std::string& image_url);

   	~MyCurl();
};
