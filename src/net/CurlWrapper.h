#pragma once
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

namespace fs = std::filesystem;

static size_t callback (void* contents, size_t size, size_t nmemb, void* userp);

class MyCurl{
private:
	CURL* curl;
	long http_code = 0;
	std::string page, buffer;
	nlohmann::json j;

	void clearning();
public:

	MyCurl();
	void get_request(std::string url);
	std::string get_data(std::string paragraph, std::string str);
	std::string download_image(const std::string& image_url);

   	~MyCurl();
};
