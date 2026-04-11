#include "CommandLineHandlers.hpp"
#include "internal/utils/string_utils.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include "wallpaper/WallpaperFactory.hpp"
#include "net/CurlWrapper.hpp"
#include "keywords/keywords.hpp"
#include "AppConfig.h"

#include <iostream>

namespace rwal::cli {

// ---------------------------------------------------------------------
// Internal handlers implementation
// ---------------------------------------------------------------------
namespace handlers {

int handleVersion() {
    std::cout << APP_NAME << " " << APP_VERSION << std::endl;
    std::cout << COPYRIGHT_STR << std::endl;
    return 0;
}

int handleContact() {
    std::string contactInfo =
        "Contact Information:\n"
        "Email: " + std::string(APP_EMAIL) + "\n"
        "Repository: " + std::string(APP_REPOSITORY_URL) + "\n"
        "Issues: " + std::string(APP_ISSUES_URL) + "\n\n"
        "Copyright: " + std::string(COPYRIGHT_STR);
    std::cout << contactInfo << std::endl;
    return 0;
}

int handleLogs(Logs& logs) {
    std::string logsContent = logs.getLogs();
    std::cout << logsContent << std::endl;
    return 0;
}

int handleClearLogs(Logs& logs) {
    if (logs.refresh()) {
        std::cout << "Logs cleared successfully." << std::endl;
    } else {
        std::cout << "Failed to clear logs." << std::endl;
    }
    return 0;
}

int handleKeywords(Config& config) {
    auto keywordsJson = config.getImpl("/search/keywords");
    std::vector<std::string> keywords;
    if (keywordsJson.is_array()) {
        keywords = keywordsJson.get<std::vector<std::string>>();
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

int handleSetKeywords(Config& config, const std::string& option) {
    if (option.empty()) {
        std::cout << "No keywords provided. Please provide comma-separated keywords." << std::endl;
        return 0;
    }
    std::string processed = option;
    rwal::utils::string::format(processed);
    std::vector<std::string> keywords = rwal::utils::string::split_by_space(processed);
    config.setImpl("/search/keywords", keywords);
    std::cout << "New keywords have been set successfully: " << processed << std::endl;
    return 0;
}

int handleAddKeywords(Config& config, const std::string& option) {
    if (option.empty()) {
        std::cout << "No keywords provided. Please provide comma-separated keywords." << std::endl;
        return 0;
    }
    std::string processed = option;
    rwal::utils::string::format(processed);
    std::vector<std::string> newKeywords = rwal::utils::string::split_by_space(processed);
    
    auto oldKeywordsJson = config.getImpl("/search/keywords");
    std::vector<std::string> oldKeywords;
    if (oldKeywordsJson.is_array()) {
        oldKeywords = oldKeywordsJson.get<std::vector<std::string>>();
    }
    oldKeywords.insert(oldKeywords.end(), newKeywords.begin(), newKeywords.end());
    config.setImpl("/search/keywords", oldKeywords);
    std::cout << "Keywords have been added successfully: " << processed << std::endl;
    return 0;
}

int handleRemoveKeywords(Config& config, const std::string& option) {
    std::string processed = option;
    rwal::utils::string::format(processed);
    std::vector<std::string> toRemove = rwal::utils::string::split_by_space(processed);
    
    auto keywordsJson = config.getImpl("/search/keywords");
    std::vector<std::string> keywords;
    if (keywordsJson.is_array()) {
        keywords = keywordsJson.get<std::vector<std::string>>();
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
    config.setImpl("/search/keywords", keywords);
    return 0;
}

int handleSave(const CLIDependencies& deps) {
    WallpaperManager wm(deps.logs, deps.fs);
    std::string message = wm.saveCurrent();
    deps.logs.writeLogs("Rwal's start for save current wallpaper");
    deps.logs.writeLogs(message);
    return 0;
}

int handleChange(const CLIDependencies& deps) {
    Keywords keywords(deps.config, deps.logs);
    auto curl = std::make_unique<CurlWrapper>(deps.logs);
    NetworkManager nm(*curl, deps.config, deps.logs);
    WallpaperFactory wf(deps.logs);
    std::unique_ptr<IWallpaperSetter> env = wf.create();
    WallpaperManager wm(deps.logs, deps.fs);
    deps.logs.writeLogs("Rwal's start in change mode");
    wm.refresh(*env, nm, keywords, nullptr, "change");
    return 0;
}

}

// ---------------------------------------------------------------------
// Public execute function
// ---------------------------------------------------------------------
int execute(const Options& opts, const CLIDependencies& deps) {
    if (opts.showVersion) return handlers::handleVersion();
    if (opts.showContact) return handlers::handleContact();
    if (opts.showLogs) return handlers::handleLogs(deps.logs);
    if (opts.clearLogs) return handlers::handleClearLogs(deps.logs);
    if (opts.showKeywords) return handlers::handleKeywords(deps.config);
    if (opts.setKeywords.has_value()) 
        return handlers::handleSetKeywords(deps.config, opts.setKeywords.value());
    if (opts.addKeywords.has_value()) 
        return handlers::handleAddKeywords(deps.config, opts.addKeywords.value());
    if (opts.removeKeywords.has_value()) 
        return handlers::handleRemoveKeywords(deps.config, opts.removeKeywords.value());
    if (opts.saveWallpaper) return handlers::handleSave(deps);
    if (opts.changeWallpaper) return handlers::handleChange(deps);
    
    // No CLI action – normal TUI mode will be run by caller
    return 0;
}

}
