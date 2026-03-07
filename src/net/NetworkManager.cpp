#include "NetworkManager.hpp"
#include "logs/logs.hpp"
#include <optional>
#include <sys/socket.h>
#include <netinet/in.h>
#include "funcs/funcs.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

struct SocketGuard{
	int fd;
	SocketGuard(int s) : fd(s){}
	~SocketGuard(){
		if (fd != -1)
			close(fd);
	}
};

NetworkManager::NetworkManager(MyCurl& curl, Config& config) : m_curl(curl), m_config(config){}

bool NetworkManager::isAvailable() {
    auto& logger = Logs::getInstance();
   	logger.write_logs("Try to check internet connection");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        logger.write_logs("Socket creation failed");
        return false;
    }

    SocketGuard guard(sock); 

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(53);
    
    if (inet_pton(AF_INET, "8.8.8.8", &server.sin_addr) <= 0) {
        logger.write_logs("IP conversion failed");
        return false;
    }

    timeval tv{.tv_sec = 3, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    if (connect(sock, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == 0) {
        logger.write_logs("Internet check: SUCCESS");
        return true;
    }

    logger.write_logs("Internet check: FAILED (No connection to 8.8.8.8:53)");
    return false;
}

std::string NetworkManager::craftUrl(std::string keyword,std::optional<std::string> page){
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
}

std::string NetworkManager::fetchImage(std::string keyword){
	int last_page;

	if (!isAvailable())
	   return "";

	m_curl.get_request(craftUrl(keyword));
	std::string pageCount =	m_curl.get_data("meta","last_page");
	

	try {
		last_page = std::stoi(pageCount);
	} catch(std::exception& e){

		Logs::getInstance().write_logs("Failed to stoi pageCount: " + std::string(e.what()));
		last_page = 1;
	}

	int max_p = std::min(last_page, 100);
	std::string page = std::to_string(random(max_p));

	try {
		m_curl.get_request(craftUrl(keyword, page));
	} catch (std::exception& e){
		Logs::getInstance().write_logs("CURL error: " + std::string(e.what()));
	}

	std::string url = m_curl.get_data("data","path");

	if (url.empty()) {
		Logs::getInstance().write_logs("No image URL found in response");
		return "";
	}
	return m_curl.download_image(url);
}
