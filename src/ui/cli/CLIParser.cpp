#include "CLI.hpp"

#include <iostream>
#include <string_view>
#include <unordered_map>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

void CLI::parse(int argc, char* argv[]) {
    std::unordered_map<std::string_view, std::function<void()>> flagMap = {
        {"-h", [&] { m_opts.showHelp = true; }},
        {"--help", [&] { m_opts.showHelp = true; }},
        {"-v", [&] { m_opts.showVersion = true; }},
        {"--version", [&] { m_opts.showVersion = true; }},
        {"-c", [&] { m_opts.changeWallpaper = true; }},
        {"--change", [&] { m_opts.changeWallpaper = true; }},
        {"-s", [&] { m_opts.saveWallpaper = true; }},
        {"--save", [&] { m_opts.saveWallpaper = true; }},
        {"--contact", [&] { m_opts.showContact = true; }},
        {"--log", [&] { m_opts.showLogs = true; }},
        {"--clear-logs", [&] { m_opts.clearLogs = true; }},
        {"-k", [&] { m_opts.showKeywords = true; }},
        {"--keywords", [&] { m_opts.showKeywords = true; }},
        {"--scheduler", [&] { m_opts.showScheduler = true; }},
    };

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        // Value-taking options
        if (arg == "--set-keywords" && i + 1 < argc) {
            m_opts.setKeywords = argv[++i];
        } else if (arg == "--add-keywords" && i + 1 < argc) {
            m_opts.addKeywords = argv[++i];
        } else if (arg == "--remove-keywords" && i + 1 < argc) {
            m_opts.removeKeywords = argv[++i];
        } else if (arg == "--set-scheduler" && i + 1 < argc) {
            m_opts.setScheduler = argv[++i];
        } else {
            // Boolean flags
            auto it = flagMap.find(arg);
            if (it != flagMap.end()) {
                it->second(); // invoke the lambda
            } else {
                m_deps.logs.writeLogs(
                    lvl::Error, mod::UI, "Failed to parse CLI, unknown flag: " + std::string(arg));
                std::cerr << "Failed to parse CLI, unknown flag: " << std::string(arg)
                          << "\n Use --help to show available options.\n";
            }
        }
    }
}
