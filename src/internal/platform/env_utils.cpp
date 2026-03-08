#include "env_utils.hpp"
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace rwal::platform::executor {
    void open_editor(fs::path& path) {
        const char* editor_env = std::getenv("EDITOR");
        std::string editor = (editor_env) ? editor_env : "nano";
        std::string path_str = path.string();

        pid_t pid = fork();

        if (pid == -1)
            return;

        if (pid == 0) {
			if (std::system("stty sane") != 0) {}
            const char* args[] = { editor.c_str(), path_str.c_str(), nullptr };
            execvp(args[0], const_cast<char**>(args));
            
            std::_Exit(1); 
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}
