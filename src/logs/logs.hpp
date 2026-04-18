#pragma once

#include <string>
#include <string_view>
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

	/*
	Q: Why pass string_view by value instead of const&?
	A: string_view is just {ptr, size} (16 bytes). Copying it is cheaper than
		dereferencing a pointer (8 bytes + indirection cost). Modern ABIs pass
		small structs in registers - this is zero-cost at the hardware level.
	*/
    virtual void writeLogs(std::string_view type, std::string_view module, std::string_view message);
	virtual std::string getLogs(const int& LinesCount = 100) const;
};

