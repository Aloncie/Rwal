#include "CurlWrapper.hpp"
#include "internal/GlobalConstans.hpp"
#include "logs/logs.hpp"
#include "funcs/funcs.hpp"

#include <exception>
#include <filesystem>
#include <string>
#include <fstream>

using CurlRaiiPtr = std::unique_ptr<CURL, CurlWrapper::CurlDeleter>;

static size_t callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    std::string* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), realsize);
    return realsize;
}

CurlWrapper::CurlWrapper() : curl(curl_easy_init(), curl_easy_cleanup) {
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

void CurlWrapper::getRequest(std::string url) {
    clearning();
    CURLcode res;
    if (curl) {
        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, &callback);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &buffer);

        res = curl_easy_perform(curl.get());

        curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &http_code);
        Logs::getInstance().writeLogs("url of request: " + url);

        if (res != CURLE_OK) {
            std::string errStr = curl_easy_strerror(res);
            Logs::getInstance().writeLogs("Curl Error: " + errStr);
            return;
        }

        if (http_code != 200) {
            Logs::getInstance().writeLogs("HTTP Error: " + std::to_string(http_code));
            return;
        } else
            Logs::getInstance().writeLogs("Successful request");

        Logs::getInstance().writeLogs("Try to parse");
        try {
            j = nlohmann::json::parse(buffer);
            Logs::getInstance().writeLogs("Successful parse JSON");
        } catch (const std::exception& e) {
            Logs::getInstance().writeLogs("JSON parse error: " + std::string(e.what()));
            return;
        }
    } else
        return;
}

std::string CurlWrapper::getData(std::string paragraph, std::string str) {
    if (!j.contains(paragraph)) {
        Logs::getInstance().writeLogs("Error: paragraph " + paragraph + " not found in JSON");
        return "";
    }
    try {
        if (j[paragraph].is_array()) {
            for (auto& item : j[paragraph]) {
                if (item.contains(str)) {
                    if (item[str].is_string()) {
                        Logs::getInstance().writeLogs("Data of JSON: " +
                                                      item[str].get<std::string>());
                        return item[str].get<std::string>();
                    }
                    Logs::getInstance().writeLogs("Data of JSON: " +
                                                  std::to_string(item[str].get<int>()));
                    return std::to_string(item[str].get<int>());
                }
            }
        }

        else if (j[paragraph].is_object()) {
            if (j[paragraph].contains(str)) {
                if (j[paragraph][str].is_string()) {
                    Logs::getInstance().writeLogs("Data of JSON: " +
                                                  j[paragraph][str].get<std::string>());
                    return j[paragraph][str].get<std::string>();
                }

                Logs::getInstance().writeLogs("Data of JSON: " +
                                              std::to_string(j[paragraph][str].get<int>()));
                return std::to_string(j[paragraph][str].get<int>());
            }
        }
    } catch (const std::exception& e) {
        Logs::getInstance().writeLogs("JSON parsing error: " + std::string(e.what()));
    }
    return "";
}

std::optional<fs::path> CurlWrapper::downloadImage(const std::string& image_url) {
    const char* home = std::getenv("HOME");
    fs::path base_path = home ? fs::path(home) : fs::path("/tmp");
    fs::path downloads = base_path / ".local/share/Aloncie/Rwal" / rwal::wallpaper::DONWLOADS_DIR_NAME;

    try {
        if (!fs::exists(downloads)) {
            fs::create_directories(downloads);
        } else {
            Logs::getInstance().writeLogs("Cleaning old images");
            for (const auto& entry : fs::directory_iterator(downloads)) {
                if (fs::is_regular_file(entry.path())) {
                    fs::remove(entry.path());
                }
            }
            Logs::getInstance().writeLogs("Successful cleanup");
        }
    } catch (const fs::filesystem_error& e) {
        Logs::getInstance().writeLogs("Filesystem Error: " + std::string(e.what()));
        return std::nullopt;
    }

    std::string filename = call_Image(image_url);
    fs::path wallpaper_path = downloads / filename;

    CurlRaiiPtr image_curl(curl_easy_init());
    if (!image_curl) {
        Logs::getInstance().writeLogs("Failed to init CURL");
        return std::nullopt;
    }

    std::ofstream fp(wallpaper_path, std::ios::binary);
    if (!fp.is_open()) {
        Logs::getInstance().writeLogs("Failed to open file for writing: " + wallpaper_path.string());
        return std::nullopt;
    }

    curl_easy_setopt(image_curl.get(), CURLOPT_URL, image_url.c_str());
    curl_easy_setopt(
        image_curl.get(), CURLOPT_WRITEFUNCTION,
        +[](void* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
            auto* stream = static_cast<std::ofstream*>(userdata);
            size_t totalSize = size * nmemb;
            stream->write(static_cast<char*>(ptr), totalSize);
            return totalSize;
        });
    curl_easy_setopt(image_curl.get(), CURLOPT_WRITEDATA, &fp);
    curl_easy_setopt(image_curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(image_curl.get(), CURLOPT_TIMEOUT, 15L);

    CURLcode res = curl_easy_perform(image_curl.get());
    fp.close();

    if (res != CURLE_OK) {
        Logs::getInstance().writeLogs("Download error: " + std::string(curl_easy_strerror(res)));
        return std::nullopt;
    }

    Logs::getInstance().writeLogs("Successful download: " + wallpaper_path.string());
    return wallpaper_path;
}

void CurlWrapper::clearning() {
    buffer.clear();
    j.clear();
}

void CurlWrapper::generateUniqueSuffix(std::string& filename) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < rwal::wallpaper::SUFFIX_LENGTH; ++i) {
        filename += chars[random(chars.size() - 1)];
    }
}

std::string CurlWrapper::call_Image(const std::string& image_url) {
    size_t lastSlash = image_url.find_last_of('/');
    std::string filename = rwal::wallpaper::FILE_PREFIX;

    if (lastSlash == std::string::npos) {
        generateUniqueSuffix(filename);
        filename += ".jpg";
    } else {
        std::string raw_tail = image_url.substr(lastSlash + 1);
        size_t firstDash = raw_tail.find('-');

        if (firstDash == std::string::npos) {
            generateUniqueSuffix(filename);

            if (raw_tail.find(".png") != std::string::npos) {
                filename += ".png";
            } else {
                filename += ".jpg";
            }
        } else {
            filename += raw_tail.substr(firstDash + 1);
        }
    }
    return filename;
}

