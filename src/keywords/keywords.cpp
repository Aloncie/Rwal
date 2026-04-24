#include "keywords.hpp"
#include "logs/logs.hpp"
#include "internal/platform/env_utils.hpp"
#include "funcs/funcs.hpp"
#include "internal/utils/string_utils.hpp"
#include "internal/utils/vector_utils.hpp"

#include <fstream>
#ifndef _WIN32
	#include <curses.h>
#endif

Keywords::Keywords(IConfigReader& config, Logs& logs) : m_config(config), m_logs(logs) {}

std::vector<std::string> Keywords::loadKeywordsFromConfig() const {
    auto search = m_config.get<nlohmann::json>("/search");
    if (search.contains("keywords") && search["keywords"].is_array()) {
        return search["keywords"].get<std::vector<std::string>>();
    }
    return {};
}

std::string Keywords::pickRandomKeyword(const std::vector<std::string>& keywords) const {
	if (keywords.empty()) return "";
	if (keywords.size() == 1) return keywords[0];
	return keywords[random(keywords.size() - 1)];
}

std::string Keywords::SilentGetKeyword() {
	std::vector<std::string> keywords = loadKeywordsFromConfig();
	if (keywords.empty()) {
		Default(keywords);
		m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Keywords, "No keywords found in config. Using default keywords.");
	}
    return pickRandomKeyword(keywords);
}

void Keywords::Default(std::vector<std::string>& keywords) const {
    m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Keywords, "Use default keywords.");
    keywords = {"nature", "landscape", "abstract", "space",
                "architecture", "animals", "anime", "cars"};
}

void Keywords::promptForKeywords(std::function<void(std::vector<std::string>)> callback, IUserInterface& ui, int attempts) {
	// Prevent infinite recursion if user keeps entering invalid input.
	if (attempts > 100) {
		ui.showMessage("Too many failed attempts. Using default keywords.");
		std::vector<std::string> defaults;
		Default(defaults);
		callback(defaults);
		return;
	}
	ui.requestInput(
		[this, callback, &ui, &attempts](std::string input) {
			rwal::utils::string::format(input);
			auto keywords = rwal::utils::string::split_by_space(input);
			if (keywords.empty()) {
				ui.showMessage("Input cannot be empty! Try again.");
				promptForKeywords(callback, ui, attempts + 1);
			} else {
				m_config.set("search", nlohmann::json{{"keywords", keywords}});
				callback(keywords);
			}
		}, "Keywords not found. Enter keywords (space separated): "); 
}



std::string Keywords::InteractiveGetKeyword(IUserInterface& ui) {
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

void Keywords::editKeywords(IUserInterface& ui) {
	fs::path temp_path = fs::temp_directory_path() / "keywords.txt";

	std::ofstream ofile(temp_path);
	if (!ofile.is_open()) return;

	auto config_keywords = loadKeywordsFromConfig();
	for (const auto& kw : config_keywords) {
		ofile << kw << "\n";
	}
	ofile.close();
	
	if (!ui.prepareForExternalCommand()) {
		ui.showMessage("Failed to open editor");
		return;
	}

	rwal::platform::executor::open_editor(temp_path);

	if (!ui.refresh()){
		ui.showMessage("Failed to refresh UI");
        return;
	}

	std::ifstream ifile(temp_path);
	std::string line;
	std::vector<std::string> input_keywords;

	if (ifile.is_open()) {
		while (std::getline(ifile, line)) {
			if (line.empty() || line[0] == '#') continue;
			rwal::utils::string::format(line);
			if (!line.empty() && line.length() < 256) {
				rwal::utils::vector::add_unique(input_keywords, line);
			}
		}
		ifile.close();
	} else {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Keywords, "Failed opening keywords.txt in " + temp_path.string());
		ui.showMessage("Failed operation. More info in logs");
	}

	m_config.set("search", nlohmann::json{{"keywords", input_keywords}});
	fs::remove(temp_path);
}

