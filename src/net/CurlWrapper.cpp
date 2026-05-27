#include "CurlWrapper.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include "internal/AppConstants.hpp"
#include "internal/AppConfig.h.in"

#include "funcs/funcs.hpp"
#include <exception>
#include <filesystem>
#include <string>
#include <fstream>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

using CurlPtr = std::unique_ptr<CURL, CurlDeleter>;

CurlWrapper::CurlWrapper(Logs& logs, IFileSystem& fs) : m_logs(logs), m_fs(fs), curl(curl_easy_init(), CurlDeleter{}) {
	curl_global_init(CURL_GLOBAL_DEFAULT);
	if (!curl) {
		m_logs.writeLogs(lvl::Error, mod::Network, "Failed to init CURL");
	}
}

CurlWrapper::~CurlWrapper() {
	curl_global_cleanup();	
}

static size_t callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    std::string* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), realsize);
    return realsize;
}

void CurlWrapper::getRequest(const std::string& url) {
    clearning();
    CURLcode res;
    if (curl) {
        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, &callback);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &buffer);

        res = curl_easy_perform(curl.get());

        curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &http_code);

        if (res != CURLE_OK) {
            std::string errStr = curl_easy_strerror(res);
			m_logs.writeLogs(lvl::Error, mod::Network, "Request error: " + errStr);
            return;
        }

        if (http_code != 200) {
			m_logs.writeLogs(lvl::Error, mod::Network, "Error HTTP code: " + std::to_string(http_code));
            return;
        }
		m_logs.writeLogs(lvl::Info, mod::Network, "Successful request");

        try {
            j = nlohmann::json::parse(buffer);
            m_logs.writeLogs(lvl::Info, mod::Network, "Successful parse JSON");
        } catch (const std::exception& e) {
            m_logs.writeLogs(lvl::Error, mod::Network, "JSON parse error: " + std::string(e.what()));
            return;
        }
    } else {
		m_logs.writeLogs(lvl::Error, mod::Network, "Failed to init CURL");
	}
}

std::string CurlWrapper::getData(const std::string& paragraph, const std::string& str) {
    if (!j.contains(paragraph)) {
        m_logs.writeLogs(lvl::Error, mod::Network, "JSON does not contain paragraph: " + paragraph);
        return "";
    }
    try {
        if (j[paragraph].is_array()) {
            for (auto& item : j[paragraph]) {
                if (item.contains(str)) {
                    if (item[str].is_string()) {
                        m_logs.writeLogs(lvl::Debug, mod::Network, "Data of JSON: " + item[str].get<std::string>());
                        return item[str].get<std::string>();
                    }
                    m_logs.writeLogs(lvl::Debug, mod::Network, "Data of JSON: " + std::to_string(item[str].get<int>()));
                    return std::to_string(item[str].get<int>());
                }
            }
        }

        else if (j[paragraph].is_object()) {
            if (j[paragraph].contains(str)) {
                if (j[paragraph][str].is_string()) {
                    m_logs.writeLogs(lvl::Debug, mod::Network, "Data of JSON: " + j[paragraph][str].get<std::string>());
                    return j[paragraph][str].get<std::string>();
                }

                m_logs.writeLogs(lvl::Debug, mod::Network, "Data of JSON: " + std::to_string(j[paragraph][str].get<int>()));
                return std::to_string(j[paragraph][str].get<int>());
            }
        }
    } catch (const std::exception& e) {
        m_logs.writeLogs(lvl::Error, mod::Network, "JSON parse error: " + std::string(e.what()));
    }
    return "";
}

std::optional<fs::path> CurlWrapper::downloadImage(const std::string& image_url) {
	fs::path base_path = m_fs.getAppLocalDataLocation() / ORGANIZATION_NAME / APP_NAME;
	if (!m_fs.existsDirectory(base_path)) {
		m_logs.writeLogs(lvl::Error, mod::Network, "Directory not found: " + base_path.string());
		m_fs.createDirectories(base_path);
	}

    fs::path downloads = base_path / rwal::constants::dirs::DOWNLOADS_DIR;

    try {
        if (!m_fs.exists(downloads)) {
            m_fs.createDirectories(downloads);
        } else {
            m_logs.writeLogs(lvl::Info, mod::Network, "Try to cleanup");
			std::vector<fs::path> downloads_files = m_fs.listDirectory(downloads);
			for (auto i : downloads_files) {
				m_fs.remove(i);
            }
            m_logs.writeLogs(lvl::Info, mod::Network, "Successful cleanup");
        }
    } catch (const fs::filesystem_error& e) {
        m_logs.writeLogs(lvl::Error, mod::Network, "Filesystem error: " + std::string(e.what()));
        return std::nullopt;
    }

    std::string filename = getFilenameFromUrl(image_url);
    fs::path wallpaper_path = downloads / filename;

    CurlPtr image_curl(curl_easy_init());
    if (!image_curl) {
        m_logs.writeLogs(lvl::Error, mod::Network, "Failed to init CURL");
        return std::nullopt;
    }

    std::ofstream fp(wallpaper_path, std::ios::binary);
    if (!fp.is_open()) {
        m_logs.writeLogs(lvl::Error, mod::Network, "Failed to open file");
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
        m_logs.writeLogs(lvl::Error, mod::Network, "Failed to download image: " + std::string(curl_easy_strerror(res)));
        return std::nullopt;
    }

    m_logs.writeLogs(lvl::Info, mod::Network, "Successful download image: " + wallpaper_path.string());
    return wallpaper_path;
}

void CurlWrapper::clearning() {
    buffer.clear();
    j.clear();
}

void CurlWrapper::generateUniqueSuffix(std::string& filename) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < rwal::constants::wallpaper::SUFFIX_LENGTH; ++i) {
        filename += chars[random(chars.size() - 1)];
    }
}

std::string CurlWrapper::getFilenameFromUrl(const std::string& image_url) {
    size_t lastSlash = image_url.find_last_of('/');
    std::string filename = std::string(rwal::constants::wallpaper::FILE_PREFIX);

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

