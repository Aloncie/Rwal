#pragma once
#include "internal/filesystem/IFileSystem.hpp"
#include "logs/Logs.hpp"

#include <curl/curl.h>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

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
    IFileSystem& m_fs;

protected:
    virtual void clearning();
    virtual void generateUniqueSuffix(std::string& filename);
    virtual std::string getFilenameFromUrl(const std::string& image_url);

public:
    CurlWrapper(Logs& logs, IFileSystem& fs);
    virtual ~CurlWrapper();

    virtual void getRequest(const std::string& url);
    virtual std::string getData(const std::string& paragraph, const std::string& str);
    virtual std::optional<fs::path> downloadImage(const std::string& image_url);
};
