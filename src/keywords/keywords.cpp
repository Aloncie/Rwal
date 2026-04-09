#include "keywords.hpp"
#include "logs/logs.hpp"
#include "internal/platform/env_utils.hpp"
#include "funcs/funcs.hpp"
#include "internal/utils/string_utils.hpp"
#include "internal/utils/vector_utils.hpp"

#include <fstream>
#include <curses.h>

Keywords::Keywords(IConfigReader& config, Logs& logs) : m_config(config), m_logs(logs) {}

std::vector<std::string> Keywords::loadKeywordsFromConfig() const {
    auto search = m_config.getImpl("/search");
    if (search.contains("keywords") && search["keywords"].is_array()) {
        return search["keywords"].get<std::vector<std::string>>();
    }
    return {};
}

std::string Keywords::pickRandomKeyword(const std::vector<std::string>& keywords) const {
    if (keywords.empty()) {
        std::vector<std::string> defaults;
        Default(defaults);
        if (defaults.empty()) return "art";
        return defaults[random(defaults.size() - 1)];
    }
    if (keywords.size() == 1) return keywords[0];
    return keywords[random(keywords.size() - 1)];
}

std::string Keywords::SilentGetKeyword() {
    auto keywords = loadKeywordsFromConfig();
    return pickRandomKeyword(keywords);
}

void Keywords::promptForKeywords(std::function<void(std::vector<std::string>)> callback, TUIManager& ui, int attempts) {
	// Prevent infinite recursion if user keeps entering invalid input.
	if (attempts > 100) {
		ui.showMessage("Too many failed attempts. Using default keywords.");
		std::vector<std::string> defaults;
		Default(defaults);
		callback(defaults);
		return;
	}
    ui.requestInputString(
        [this, callback, &ui, &attempts](std::string input) {
            rwal::utils::string::format(input);
            auto keywords = rwal::utils::string::split_by_space(input);
            if (keywords.empty()) {
                ui.showMessage("Input cannot be empty! Try again.");
                promptForKeywords(callback, ui, attempts + 1);
            } else {
                m_config.setImpl("search", {{"keywords", keywords}});
                callback(keywords);
            }
        }, "Keywords not found. Enter keywords (space separated): "); 
}

std::string Keywords::InteractiveGetKeyword(TUIManager& ui) {
    auto keywords = loadKeywordsFromConfig();
    if (!keywords.empty()) {
        return pickRandomKeyword(keywords);
    }

    // No keywords – prompt user, then return a random one from the entered list.
    std::string result;
    promptForKeywords(
        [this, &result](std::vector<std::string> kw) {
            result = pickRandomKeyword(kw);
        },
        ui);
    return result;
}

void Keywords::Default(std::vector<std::string>& keywords) const {
    m_logs.writeLogs("Use default keywords.");
    keywords = {"nature", "landscape", "abstract", "space",
                "architecture", "animals", "anime", "cars"};
}

void Keywords::editKeywords(TUIManager& ui) {
    fs::path temp_path = fs::temp_directory_path() / "keywords.txt";
    importToTxt(temp_path);

    def_prog_mode();
    endwin();

    rwal::platform::executor::open_editor(temp_path);

    refresh();

    auto keywords = exportFromTxt(temp_path, ui);
    m_config.setImpl("search", {{"keywords", keywords}});
    fs::remove(temp_path);
}

void Keywords::importToTxt(const fs::path& path) const {
    std::ofstream file(path);
    if (!file.is_open()) return;

    auto keywords = loadKeywordsFromConfig();
    for (const auto& kw : keywords) {
        file << kw << "\n";
    }
    file.close();
}

std::vector<std::string> Keywords::exportFromTxt(const fs::path& path, TUIManager& ui) const {
    std::ifstream file(path);
    std::string line;
    std::vector<std::string> keywords;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            rwal::utils::string::format(line);
            if (!line.empty() && line.length() < 256) {
                rwal::utils::vector::add_unique(keywords, line);
            }
        }
        file.close();
    } else {
        m_logs.writeLogs("Failed opening keywords.txt in " + path.string());
        ui.showMessage("Failed operation. More info in logs");
    }
    return keywords;
}
