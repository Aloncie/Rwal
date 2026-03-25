#include "PathResolver.hpp"

#include <string>

void PathResolver::toHostPath(fs::path& path) {
    const char* host_home = std::getenv("HOST_HOME");
    const char* home = std::getenv("HOME");
    if (host_home == home)
        return;
    else {
        std::string pathStr = path.string();
        if (home && host_home) {
            std::string c_home(home);
            if (pathStr.find(c_home) == 0) {
                pathStr.replace(0, c_home.length(), std::string(host_home));
            }
        }
        path = fs::path(pathStr);
    }
}
