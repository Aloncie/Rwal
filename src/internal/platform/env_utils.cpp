#include "env_utils.hpp"
#include "internal/AppConstants.hpp"

#include <cstdlib>
#include <sys/wait.h>

#ifdef _WIN32
	#include <Windows.h>
	#include <shellapi.h>
#else
	#include <sys/wait.h>
	#include <unistd.h>
#endif

namespace rwal::platform::executor {
    void open_editor(fs::path& path) {
#ifdef _WIN32
		std::wstring widePath = path.wstring();
		SHELLEXECUTEINFOW sei = { sizeof(sei) };
		sei.lpFile = widePath.c_str();
		sei.nShow = SW_SHOWNORMAL;
		sei.fMask = SEE_MASK_FLAG_NO_UI;
		ShellExecuteExW(&sei);
#else
        const char* editor_env = std::getenv("EDITOR");
        std::string editor = (editor_env) ? editor_env : "nano";
        std::string path_str = path.string();

        pid_t pid = fork();

        if (pid == -1)	return;

        if (pid == 0) {
			if (std::system("stty sane") != 0) {}
            const char* args[] = { editor.c_str(), path_str.c_str(), nullptr };
            execvp(args[0], const_cast<char**>(args));
            
            std::_Exit(1); 
        } else {
            int status;
            waitpid(pid, &status, 0);
#endif
        }
    }
}

namespace rwal::systemd{
	int exec(const std::string& command){
		try {
			std::string full = command + std::string(rwal::constants::systemd::SUPPRESS_OUTPUT);
			int status = system(full.c_str());

			if (status == -1) return -1;

			return WEXITSTATUS(status);
		} catch (const std::exception& e) {
			return 1;
		}
	}
}
