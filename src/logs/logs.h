#pragma once
#include <memory>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>

class Logs{
private:
	std::string get_current_time();
public:
	void write_logs(std::string message);
};
