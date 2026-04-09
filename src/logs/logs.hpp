#pragma once
#include "ui/cli/UIManager.hpp"

#include <string>
#include <filesystem>
#include <fstream>
#include <QStandardPaths>

namespace fs = std::filesystem;

class Logs {
private:
    fs::path logs_path;
    std::ofstream f;
  
    std::string getCurrentTime();
    void refresh(fs::path& logs_path);

public:
    Logs(); 
    virtual void writeLogs(std::string_view message);
	virtual std::string getLogs(int LinesCount = 100);
};

