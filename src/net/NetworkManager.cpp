#include "NetworkManager.hpp"
#include "funcs/funcs.hpp"

// Undefine any existing timeout macro to avoid conflicts with socket options
#ifdef timeout
#undef timeout
#endif

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QMetaObject>
#include <QCoreApplication>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct SocketGuard{
	int fd;
	SocketGuard(int s) : fd(s){}
	~SocketGuard(){
		if (fd != -1)
			close(fd);
	}
};

bool NetworkManager::isAvailable() {
   	m_logs.writeLogs("Try to check internet connection");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        m_logs.writeLogs("Socket creation failed");
        return false;
    }

    SocketGuard guard(sock); 

    sockaddr_in server{}; server.sin_family = AF_INET;
    server.sin_port = htons(53);
    
    if (inet_pton(AF_INET, "8.8.8.8", &server.sin_addr) <= 0) {
        m_logs.writeLogs("IP conversion failed");
        return false;
    }

    timeval tv{.tv_sec = 3, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    if (connect(sock, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == 0) {
        m_logs.writeLogs("Internet check: SUCCESS");
        return true;
    }

    m_logs.writeLogs("Internet check: FAILED (No connection to 8.8.8.8:53)");
    return false;
}

std::string NetworkManager::craftUrl(std::string keyword,std::optional<std::string> page){
	try {
		auto& cfg = m_config.all();

		auto& wh = cfg["services"]["wallhaven"];
		auto& search = cfg["search"];

		std::string url = wh["base_url"].get<std::string>();
		url += wh["param_names"]["query"].get<std::string>() + keyword;
		if (page.has_value()) url += "&page=" + *page;
		url += "&" + wh["param_names"]["sorting"].get<std::string>() + "=" + search["sorting"].get<std::string>();
		url += "&" + wh["param_names"]["res"].get<std::string>() + "=" + search["res"].get<std::string>();
		url += "&" + wh["apikey"].get<std::string>();

		return url;
	} catch (std::exception& e){
		m_logs.writeLogs("Failed craft url: " + std::string(e.what()));
		return "";
	}
}

void NetworkManager::fetchImage(std::string keyword, std::function<void(std::optional<fs::path>)> callback) {
	QtConcurrent::run([this, keyword, callback](){

		int last_page;
		std::string url;

		if (!isAvailable()){
			callback(std::nullopt);
		}

		m_curl.getRequest(craftUrl(keyword));
		auto search = m_config.getImpl("/search");
		if (search.is_null()) {
			m_logs.writeLogs("Search config is missing");
			callback(std::nullopt);
			return;
		}
		else if (!search.contains("random_page") || search["random_page"].get<bool>() == false) {
			m_logs.writeLogs("Fetch image from first page");
			url = m_curl.getData("data","path");
		}
		else if (search.contains("random_page") && search["random_page"].get<bool>() == true) {
			m_logs.writeLogs("Fetch image from random page");
		
			std::string pageCount =	m_curl.getData("meta","last_page");
			
			try {
				last_page = std::stoi(pageCount);
			} catch(std::exception& e){

				m_logs.writeLogs("Failed to stoi pageCount: " + std::string(e.what()));
				last_page = 1;
			}

			int max_p = std::min(last_page, 100);
			std::string page = std::to_string(random(max_p));

			try {
				m_curl.getRequest(craftUrl(keyword, page));
			} catch (std::exception& e){
				m_logs.writeLogs("CURL error: " + std::string(e.what()));
				callback(std::nullopt);
				return;
			}

			url = m_curl.getData("data","path");

		}

		if (url.empty()) {
			m_logs.writeLogs("No image URL found in response");
			callback(std::nullopt);
			return;
		}

		auto result = m_curl.downloadImage(url);
		QMetaObject::invokeMethod(QCoreApplication::instance(), [callback, result](){
			callback(result);
		});
	});
}

