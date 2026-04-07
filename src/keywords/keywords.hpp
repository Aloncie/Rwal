#pragma once
#include "ui/cli/UIManager.hpp"
#include "settings/IConfigReader.hpp"
#include "logs/logs.hpp"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

class Keywords {
private:
	IConfigReader& m_config;
	Logs& m_logs;

    void promptForKeywords(std::function<void(std::vector<std::string>)> callback, UIManager& ui, int attempts = 0);
    void importToTxt(const fs::path& path) const;
	std::vector<std::string> exportFromTxt(const fs::path& path, UIManager& ui) const;
	std::string pickRandomKeyword(const std::vector<std::string>& keywords) const;
	
public:
    explicit Keywords(IConfigReader& config, Logs& logs);
	[[nodiscard]] virtual std::vector<std::string> loadKeywordsFromConfig() const;
	[[nodiscard]] virtual std::string SilentGetKeyword();
    virtual void Default(std::vector<std::string>& keywords) const;
    void editKeywords(UIManager& ui);
    [[nodiscard]] virtual std::string InteractiveGetKeyword(UIManager& ui);
	virtual ~Keywords() = default;
};

