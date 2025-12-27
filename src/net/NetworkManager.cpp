#include "NetworkManager.h"
#include "logs/logs.h"
#include <sys/socket.h>
#include <netinet/in.h>
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

NetworkManager::NetworkManager(){
	mycurl	
}

NetworkManager& NetworkManager::getInstance(){
	static NetworkManager instance;
	return instance;
}

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

std::string NetworkManager::fetchImage(const std::string& query){
	
}
