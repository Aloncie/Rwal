#include "CLI.hpp"
#include "internal/utils/string_utils.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include "wallpaper/WallpaperFactory.hpp"
#include "net/CurlWrapper.hpp"
#include "keywords/keywords.hpp"
#include "AppConfig.h"

#include <iostream>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

// ---------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------

std::string CLI::getBinaryLocation() {
#ifdef __linux__
    return std::filesystem::read_symlink("/proc/self/exe").filename().string();
#elif defined(_WIN32)
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    return std::filesystem::path(buf).filename().string();
#else
    return "rwal";  // fallback for unknown platforms
#endif
}

// ---------------------------------------------------------------------
// Internal handlers implementation
// ---------------------------------------------------------------------

int CLI::handleHelp() {
	std::string usage = getBinaryLocation() + " [options]";
	std::cout << "Usage: " << usage;
	std::cout << R"(
Rwal - cross-platform wallpaper utility

Options:
 -h,  --help                Show this help message
 -c,  --change              Change wallpaper once and exit
 -s,  --save                Save current wallpaper
 --contact                  Show contact information
 -v,  --version             Show application version
 --log                      Show latest logs
 --clear-logs          	    Clear log file
 -k,  --keywords            Show keywords
 --scheduler                Show scheduler
 --set-keywords <kw>        Set keywords (comma-separated, e.g.: nature,ocean,town)
 --add-keywords <kw>        Add keywords (comma-separated)
 --remove-keywords <kw>     Remove keywords (comma-separated)
 --set-scheduler <sched>    Set scheduler (now available: daily, hourly, none)
	)" << std::endl;
	return 0;
}

int CLI::handleVersion() {
    std::cout << APP_NAME << " " << APP_VERSION << std::endl;
    std::cout << COPYRIGHT_STR << std::endl;
    return 0;
}

int CLI::handleContact() {
    std::string contactInfo =
        "Contact Information:\n"
        "Email: " + std::string(APP_EMAIL) + "\n"
        "Repository: " + std::string(APP_REPOSITORY_URL) + "\n"
        "Issues: " + std::string(APP_ISSUES_URL) + "\n\n"
        "Copyright: " + std::string(COPYRIGHT_STR);
    std::cout << contactInfo << std::endl;
    return 0;
}

int CLI::handleLogs() {
    std::string LogsContent = m_deps.logs.getLogs();
    std::cout << LogsContent << std::endl;
    return 0;
}

int CLI::handleClearLogs() {
    if (m_deps.logs.refresh()) {
        std::cout << "Logs cleared successfully." << std::endl;
		return 0;
    }
	std::cerr << "Failed to clear logs." << std::endl;
	return 1;
}

int CLI::handleKeywords() {
    auto keywordsJson = m_deps.config.get<nlohmann::json>("/search/keywords");
    std::vector<std::string> keywords;
    if (keywordsJson.is_array()) {
        keywords = keywordsJson.get<std::vector<std::string>>();
    } else {
		m_deps.logs.writeLogs(lvl::Warning, mod::Core, "No keywords array found in config file");
		std::cerr << "No keywords array found in config file." << std::endl;
		return 1;
	}
    if (keywords.empty()) {
        std::cout << "There are no keywords in your config file." << std::endl;
    } else {
        std::cout << "Keywords: ";
        for (size_t i = 0; i < keywords.size(); ++i) {
            std::cout << keywords[i];
            if (i < keywords.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    return 0;
}

int CLI::handleSetKeywords() {
	std::string option = m_opts.setKeywords.value();
    if (option.empty()) {
        std::cerr << "No keywords provided. Please provide comma-separated keywords." << std::endl;
        return 0;
    }
    std::string processed = option;
    rwal::utils::string::format(processed);
    std::vector<std::string> keywords = rwal::utils::string::split_by_space(processed);
    m_deps.config.set("/search/keywords", keywords);
    std::cout << "New keywords have been set successfully: " << processed << std::endl;
    return 0;
}

int CLI::handleAddKeywords() {
	std::string option = m_opts.addKeywords.value();
    if (option.empty()) {
        std::cerr << "No keywords provided. Please provide comma-separated keywords." << std::endl;
        return 0;
    }
    std::string processed = option;
    rwal::utils::string::format(processed);
    std::vector<std::string> newKeywords = rwal::utils::string::split_by_space(processed);
    
    auto oldKeywordsJson = m_deps.config.get<nlohmann::json>("/search/keywords");
    std::vector<std::string> oldKeywords;
    if (oldKeywordsJson.is_array()) {
        oldKeywords = oldKeywordsJson.get<std::vector<std::string>>();
    } else{
		m_deps.logs.writeLogs(lvl::Warning, mod::Core, "No keywords array found in config file");
		std::cerr << "No keywords array found in config file. Nothing to add.\n";
        return 1;
	}
    oldKeywords.insert(oldKeywords.end(), newKeywords.begin(), newKeywords.end());
    m_deps.config.set("/search/keywords", oldKeywords);
    std::cout << "Keywords have been added successfully: " << processed << std::endl;
    return 0;
}

int CLI::handleRemoveKeywords() {
    std::string processed = m_opts.removeKeywords.value();
    rwal::utils::string::format(processed);
    std::vector<std::string> toRemove = rwal::utils::string::split_by_space(processed);
    
    auto keywordsJson = m_deps.config.get<nlohmann::json>("/search/keywords");
    std::vector<std::string> keywords;
    if (keywordsJson.is_array()) {
        keywords = keywordsJson.get<std::vector<std::string>>();
    } else {
		m_deps.logs.writeLogs(lvl::Warning, mod::Core, "No keywords array found in config file");
		std::cerr << "No keywords array found in config file. Nothing to remove.\n";
        return 1;
	}
    
    std::string removedStr = "Removed keywords: ";
    for (const auto& rem : toRemove) {
        for (auto it = keywords.begin(); it != keywords.end(); ) {
            if (*it == rem) {
                removedStr += *it + " ";
                it = keywords.erase(it);
            } else {
                ++it;
            }
        }
    }
    std::cout << removedStr << std::endl;
    m_deps.config.set("/search/keywords", keywords);
    return 0;
}

int CLI::handleSave() {
	m_deps.logs.writeLogs(lvl::Info, mod::Core, "Rwal's start for save current wallpaper");
    WallpaperManager wm(m_deps.logs, m_deps.fs);
    std::string message = wm.saveCurrent();
    m_deps.logs.writeLogs(lvl::Info, mod::Wallpaper, message);
    return 0;
}

int CLI::handleChange() {
	m_deps.logs.writeLogs(lvl::Info, mod::Core, "Rwal's start in change mode");
    Keywords keywords(m_deps.config, m_deps.logs, m_deps.fs);
    auto curl = std::make_unique<CurlWrapper>(m_deps.logs, m_deps.fs);
    NetworkManager m_netmanager(*curl, m_deps.config, m_deps.logs);
    std::unique_ptr<IWallpaperSetter> env = createWallpaperSetter(m_deps.logs);
    WallpaperManager wm(m_deps.logs, m_deps.fs);
    wm.refresh(*env, m_netmanager, keywords, nullptr, "change");
    return 0;
}

int CLI::handleScheduler() {
	m_deps.logs.writeLogs(lvl::Info, mod::Core, "Rwal's start for show scheduler");
	auto gettingType = m_deps.scheduler.get();
	if (!gettingType.has_value()){
		std::cerr << "Failed to get scheduler. More info in logs" << std::endl;
	} else {
		std::string typeStr = rwal::system::Scheduler::toString(gettingType.value()).value_or("Unknown type");
		std::cout << "Current scheduler: " << typeStr << std::endl;
	}
	return 0;
}

int CLI::handleSetScheduler() {
	m_deps.logs.writeLogs(lvl::Info, mod::Core, "Rwal's start for set scheduler");
	auto inputType = rwal::system::Scheduler::toType(m_opts.setScheduler.value());
	if (!inputType.has_value()){
		std::cerr << "Invalid scheduler type. Please provide valid type." << std::endl;
	} else {
		std::string answer = m_deps.scheduler.set(inputType.value());
		if (answer == "Failed to set task. More info in logs."){
			std::cerr << "Function of setting scheduler was failed. More info in logs" << std::endl;
		} else {
			std::cout << answer << std::endl;
		}
	}
	return 0;
}

// ---------------------------------------------------------------------
// Public execute function
// ---------------------------------------------------------------------
int CLI::execute() {
	if (m_opts.showHelp) return handleHelp();
    if (m_opts.showVersion) return handleVersion();
    if (m_opts.showContact) return handleContact();
    if (m_opts.showLogs) return handleLogs();
    if (m_opts.clearLogs) return handleClearLogs();
    if (m_opts.showKeywords) return handleKeywords();
	if (m_opts.saveWallpaper) return handleSave();
    if (m_opts.changeWallpaper) return handleChange();
	if (m_opts.showScheduler) return handleScheduler();

    if (m_opts.setKeywords.has_value()) 
        return handleSetKeywords();
    if (m_opts.addKeywords.has_value()) 
        return handleAddKeywords();
    if (m_opts.removeKeywords.has_value()) 
        return handleRemoveKeywords();
	if (m_opts.setScheduler.has_value())
		return handleSetScheduler();
       
    // No CLI action – TUI mode will be run by caller
    return 0;
}
