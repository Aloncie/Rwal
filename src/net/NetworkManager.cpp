#include "NetworkManager.hpp"
#include "logs/logs.hpp"
#include "funcs/funcs.hpp"
#include "settings/IConfigReader.hpp"

#include <optional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

struct SocketGuard{
	int fd;
	SocketGuard(int s) : fd(s){}
	~SocketGuard(){
		if (fd != -1)
			close(fd);
	}
};

NetworkManager::NetworkManager(CurlWrapper& curl, IConfigReader& config) : m_curl(curl), m_config(config){}

bool NetworkManager::isAvailable() {
    auto& logger = Logs::getInstance();
   	logger.writeLogs("Try to check internet connection");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        logger.writeLogs("Socket creation failed");
        return false;
    }

    SocketGuard guard(sock); 

    sockaddr_in server{}; server.sin_family = AF_INET;
    server.sin_port = htons(53);
    
    if (inet_pton(AF_INET, "8.8.8.8", &server.sin_addr) <= 0) {
        logger.writeLogs("IP conversion failed");
        return false;
    }

    timeval tv{.tv_sec = 3, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    if (connect(sock, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == 0) {
        logger.writeLogs("Internet check: SUCCESS");
        return true;
    }

    logger.writeLogs("Internet check: FAILED (No connection to 8.8.8.8:53)");
    return false;
}

std::string NetworkManager::craftUrl(std::string keyword,std::optional<std::string> page){
	try {
		auto& cfg = m_config.all();

		auto& wh = cfg["services"]["wallhaven"];
		auto& search = cfg["search"];

		std::string url = wh["base_url"].get<std::string>();
		url += wh["param_names"]["query"].get<std::string>() + keyword;
		if (page.has_value())
			url += "&page=" + *page;
		url += "&" + wh["param_names"]["sorting"].get<std::string>() + "=" + search["sorting"].get<std::string>();
		url += "&" + wh["param_names"]["res"].get<std::string>() + "=" + search["res"].get<std::string>();
		url += "&" + wh["apikey"].get<std::string>();

		return url;
	} catch (std::exception& e){
		Logs::getInstance().writeLogs("Failed craft url: " + std::string(e.what()));
		return "";
	}
}

std::optional<fs::path> NetworkManager::fetchImage(std::string keyword){
	int last_page;
	std::string url;

	if (!isAvailable()){
	   return std::nullopt;
	}

	m_curl.getRequest(craftUrl(keyword));
	auto search = m_config.getImpl("/search");
	if (search.is_null()) {
		Logs::getInstance().writeLogs("Search config is missing");
		return std::nullopt;
	}
	else if (!search.contains("random_page") || search["random_page"].get<bool>() == false) {
		Logs::getInstance().writeLogs("Fetch image from first page");
		url = m_curl.getData("data","path");
	}
	else if (search.contains("random_page") && search["random_page"].get<bool>() == true) {
		Logs::getInstance().writeLogs("Fetch image from random page");
	
		std::string pageCount =	m_curl.getData("meta","last_page");
		
		try {
			last_page = std::stoi(pageCount);
		} catch(std::exception& e){

			Logs::getInstance().writeLogs("Failed to stoi pageCount: " + std::string(e.what()));
			last_page = 1;
		}

		int max_p = std::min(last_page, 100);
		std::string page = std::to_string(random(max_p));

		try {
			m_curl.getRequest(craftUrl(keyword, page));
		} catch (std::exception& e){
			Logs::getInstance().writeLogs("CURL error: " + std::string(e.what()));
			return std::nullopt;
		}

		url = m_curl.getData("data","path");

	}

	if (url.empty()) {
		Logs::getInstance().writeLogs("No image URL found in response");
		return std::nullopt;
	}
	return m_curl.downloadImage(url);
}

