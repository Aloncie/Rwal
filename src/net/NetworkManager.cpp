#define NOMINMAX // Must be before any Windows headers
#include "NetworkManager.hpp"
#include "AppConfig.h"
#include "funcs/funcs.hpp"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <format>
#endif

#include <algorithm>
#include <format>
#include <vector>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

#ifndef _WIN32
struct SocketGuard{
    int fd;
    SocketGuard(int s) : fd(s) {}
    ~SocketGuard() {
        if (fd != -1)
            close(fd);
    }
};
#endif

bool NetworkManager::isAvailable() {
    m_logs.writeLogs(lvl::Debug, mod::Network, "Try to check internet connection");

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        m_logs.writeLogs(lvl::Error, mod::Network, "WSAStartup failed");
        return false;
    }
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        m_logs.writeLogs(lvl::Error, mod::Network, "Socket creation failed");
        WSACleanup();
        return false;
    }
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &server.sin_addr);
    int timeout = 3000;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
    int result = connect(sock, (sockaddr*)&server, sizeof(server));
    bool connected = (result == 0);
    closesocket(sock);
    WSACleanup();
    if (connected) {
        m_logs.writeLogs(lvl::Info, mod::Network, "Internet check: SUCCESS");
        return true;
    } else {
        m_logs.writeLogs(lvl::Warning, mod::Network, "Internet check: FAILED");
        return false;
    }
#else
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        m_logs.writeLogs(lvl::Error, mod::Network, "Socket creation failed");
        return false;
    }
    SocketGuard guard(sock); 
    sockaddr_in server{}; server.sin_family = AF_INET;
    server.sin_port = htons(53);
    if (inet_pton(AF_INET, "8.8.8.8", &server.sin_addr) <= 0) {
        m_logs.writeLogs(lvl::Error, mod::Network, "IP conversion failed");
        return false;
    }
    timeval tv{.tv_sec = 3, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    if (connect(sock, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == 0) {
        m_logs.writeLogs(lvl::Info, mod::Network, "Internet check: SUCCESS");
        return true;
    }
    m_logs.writeLogs(lvl::Warning, mod::Network, "Internet check: FAILED (No connection to 8.8.8.8:53)");
    return false;
#endif
}

std::string NetworkManager::craftUrl(std::string_view keyword, const std::optional<std::string>& page) {
    try {
        auto& cfg = m_config.all();
        auto& wh = cfg["services"]["wallhaven"];
        auto& search = cfg["search"];
		auto& p_names = wh["param_names"];

        std::vector<std::string> params;
		
		auto add_param = [&](std::string_view name, std::string_view value) {
			params.push_back(std::format("{}={}", name, value));
		};
		// Must-have parametr
        add_param(p_names["query"].get<std::string>(), keyword);        
        if (page) {
			add_param(p_names["page"].get<std::string>(), *page);
        }
        
		add_param(p_names["sorting"].get<std::string>(), search["sorting"].get<std::string>());
		add_param(p_names["res"].get<std::string>(), search["res"].get<std::string>());
        
        if (!wh["apikey"].get<std::string>().empty()) {
			add_param(p_names["apikey"].get<std::string>(), wh["apikey"].get<std::string>());
        }

        std::string url = wh["base_url"].get<std::string>() + "?";
        for (size_t i = 0; i < params.size(); ++i) {
            url += (i > 0 ? "&" : "") + params[i];
        }
        return url;
    } catch (std::exception& e) {
        m_logs.writeLogs(lvl::Error, mod::Network, "Failed craft url: " + std::string(e.what()));
        return "";
    }
}
std::optional<fs::path> NetworkManager::fetchImage(std::string_view keyword) {
	int last_page;
	std::string url;

	if (!isAvailable()){
		m_logs.writeLogs(lvl::Info, mod::Network, "Network isn't available, using local wallpapers.");
		fs::path pictures_path = m_fs.getPicturesLocation() / APP_NAME;
		std::string error = m_fs.getLastError();
		if (error != "") {
			m_logs.writeLogs(lvl::Error, mod::Network, "Failed to get pictures directory: " + error);
			return std::nullopt;
		} 

		bool exists = m_fs.existsDirectory(pictures_path);
		error = m_fs.getLastError();
		if (!exists){
			m_logs.writeLogs(lvl::Error, mod::Network, "Pictures directory doesn't exist");
			return std::nullopt;
		} else if (error != "") {
			m_logs.writeLogs(lvl::Error, mod::Network, "Failed to get wallpapers path: " + error);
			return std::nullopt;
		}

		std::vector<fs::path> wallpapers = m_fs.listDirectory(pictures_path, "wallpaper");
		error = m_fs.getLastError();
		if (error != "") {
			m_logs.writeLogs(lvl::Error, mod::Network, "Failed to get wallpapers list: " + error);
			return std::nullopt;
		}

		// random() take any number from 1 to N 
		// so we need to keep wallpapers.size() without -1 and then minus 1 to get the index
		// without it we can't get null element
		int num = random(wallpapers.size()); 

		return wallpapers[num-1];	
	}
	
	m_curl.getRequest(craftUrl(keyword));
	auto search = m_config.get<nlohmann::json>("/search");
	if (search.is_null()) {
		m_logs.writeLogs(lvl::Warning, mod::Network, "Search config is missing");
		return std::nullopt;
	}
	else if (!search.contains("random_page") || search["random_page"].get<bool>() == false) {
		m_logs.writeLogs(lvl::Debug, mod::Network, "Fetch image from first page");
		url = m_curl.getData("data","path");
	}
	else if (search.contains("random_page") && search["random_page"].get<bool>() == true) {
		m_logs.writeLogs(lvl::Debug, mod::Network, "Fetch image from random page");
	
		std::string pageCount =	m_curl.getData("meta","last_page");
		
		try {
			last_page = std::stoi(pageCount);
		} catch(std::exception& e){

			m_logs.writeLogs(lvl::Error, mod::Network, "Failed to stoi pageCount: " + std::string(e.what()));
			last_page = 1;
		}

		int max_p = std::min(last_page, 100);
		std::string page = std::to_string(random(max_p));

		try {
			m_curl.getRequest(craftUrl(keyword, page));
		} catch (std::exception& e){
			m_logs.writeLogs(lvl::Error, mod::Network, "CURL error: " + std::string(e.what()));
			return std::nullopt;
		}

		url = m_curl.getData("data","path");

	}
	else {
		m_logs.writeLogs(lvl::Warning, mod::Network, "Search config contains invalid value");
        return std::nullopt;
	}
	if (url.empty()) {
		m_logs.writeLogs(lvl::Warning, mod::Network, "No image URL found in response");
		return std::nullopt;
	}

	return m_curl.downloadImage(url);
}

