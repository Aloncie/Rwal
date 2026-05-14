#pragma once
#include "logs/logs.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

struct CurlDeleter {
	void operator()(CURL* curl) const { curl_easy_cleanup(curl); }
};

static size_t callback(void* contents, size_t size, size_t nmemb, void* userp);

class CurlWrapper {
    std::unique_ptr<CURL, CurlDeleter> curl;

    long http_code = 0;
    std::string page, buffer;
    nlohmann::json j;
	Logs& m_logs;

    void clearning();
    void generateUniqueSuffix(std::string& filename);
    std::string call_Image(const std::string& image_url);
public:
	CurlWrapper(Logs& logs);
	virtual ~CurlWrapper();
	
    virtual void getRequest(const std::string& url);
    virtual std::string getData(const std::string& paragraph, const std::string& str);
    virtual std::optional<fs::path> downloadImage(const std::string& image_url);
};

