#include "NetworkManager.hpp"
#include "funcs/funcs.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <format>

#define PLANNED_LOCAL_FETCH // Warning instead of Error, because local fetching is coming

struct SocketGuard{
	int fd;
	SocketGuard(int s) : fd(s){}
	~SocketGuard(){
		if (fd != -1)
			close(fd);
	}
};

bool NetworkManager::isAvailable() {
	m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Network, "Try to check internet connection");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Network, "Socket creation failed");
        return false;
    }

    SocketGuard guard(sock); 

    sockaddr_in server{}; server.sin_family = AF_INET;
    server.sin_port = htons(53);
    
    if (inet_pton(AF_INET, "8.8.8.8", &server.sin_addr) <= 0) {
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Network, "IP conversion failed");
        return false;
    }

    timeval tv{.tv_sec = 3, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    if (connect(sock, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == 0) {
        m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Network, "Internet check: SUCCESS");
        return true;
    }
	PLANNED_LOCAL_FETCH
    m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Network, "Internet check: FAILED (No connection to 8.8.8.8:53)");
    return false;
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
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Network, "Failed craft url: " + std::string(e.what()));
        return "";
    }
}
std::optional<fs::path> NetworkManager::fetchImage(std::string_view keyword) {
	int last_page;
	std::string url;

	if (!isAvailable()){
		return std::nullopt;
	}
	
	m_curl.getRequest(craftUrl(keyword));
	auto search = m_config.get<nlohmann::json>("/search");
	if (search.is_null()) {
		m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Network, "Search config is missing");
		return std::nullopt;
	}
	else if (!search.contains("random_page") || search["random_page"].get<bool>() == false) {
		m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Network, "Fetch image from first page");
		url = m_curl.getData("data","path");
	}
	else if (search.contains("random_page") && search["random_page"].get<bool>() == true) {
		m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Network, "Fetch image from random page");
	
		std::string pageCount =	m_curl.getData("meta","last_page");
		
		try {
			last_page = std::stoi(pageCount);
		} catch(std::exception& e){

			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Network, "Failed to stoi pageCount: " + std::string(e.what()));
			last_page = 1;
		}

		int max_p = std::min(last_page, 100);
		std::string page = std::to_string(random(max_p));

		try {
			m_curl.getRequest(craftUrl(keyword, page));
		} catch (std::exception& e){
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Network, "CURL error: " + std::string(e.what()));
			return std::nullopt;
		}

		url = m_curl.getData("data","path");

	}
	else {
		PLANNED_LOCAL_FETCH
		m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Network, "Search config contains invalid value");
        return std::nullopt;
	}
	if (url.empty()) {
		PLANNED_LOCAL_FETCH
		m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Network, "No image URL found in response");
		return std::nullopt;
	}

	return m_curl.downloadImage(url);
}

