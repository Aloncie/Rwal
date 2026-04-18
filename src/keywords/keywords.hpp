#pragma once

#include "ui/IUserInterface.hpp"
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
	
    void promptForKeywords(std::function<void(std::vector<std::string>)> callback, IUserInterface& ui, int attempts = 0);
	std::string pickRandomKeyword(const std::vector<std::string>& keywords) const;
public:
    explicit Keywords(IConfigReader& config, Logs& logs);
	[[nodiscard]] virtual std::vector<std::string> loadKeywordsFromConfig() const;
	[[nodiscard]] virtual std::string SilentGetKeyword();
    virtual void Default(std::vector<std::string>& keywords) const;
    void editKeywords(IUserInterface& ui);
	[[nodiscard]] virtual std::string InteractiveGetKeyword(IUserInterface& ui);
	virtual ~Keywords() = default;
};

