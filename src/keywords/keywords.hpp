#pragma once
#include "ui/cli/UIManager.hpp"
#include "settings/IConfigReader.hpp"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

class Keywords {
private:
	IConfigReader& m_config;

    void promptForKeywords(std::function<void(std::vector<std::string>)> callback, UIManager& ui);
    void importToTxt(const fs::path& path) const;
	std::vector<std::string> exportFromTxt(const fs::path& path, UIManager& ui) const;
	std::string pickRandomKeyword(const std::vector<std::string>& keywords) const;
	
public:
    explicit Keywords(IConfigReader& config);
	[[nodiscard]] std::vector<std::string> loadKeywordsFromConfig() const;
	[[nodiscard]] std::string SilentGetKeyword();
    void Default(std::vector<std::string>& keywords) const;
    void editKeywords(UIManager& ui);
    [[nodiscard]] std::string InteractiveGetKeyword(UIManager& ui);
};

