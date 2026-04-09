#pragma once
#include "ui/cli/UIManager.hpp"

#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class Logs {
private:
    fs::path logs_path;
    std::ofstream f;
  
    std::string getCurrentTime() const;
public:
	virtual bool refresh();

    Logs(); 
    virtual void writeLogs(std::string_view message);
	virtual std::string getLogs(const int& LinesCount = 100) const;
};

