#pragma once
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

namespace fs = std::filesystem;

static size_t callback (void* contents, size_t size, size_t nmemb, void* userp);

class MyCurl{
private:
	CURL* curl;
	int http_code = 0;
	std::string page, buffer;
	nlohmann::json j;

	void clearning();
	void generateUniqueSuffix(std::string& filename);
	std::string call_Image(const std::string& image_url);
public:

	MyCurl();
	void getRequest(std::string url);
	std::string getData(std::string paragraph, std::string str);
	std::string downloadImage(const std::string& image_url);

   	~MyCurl();
};
