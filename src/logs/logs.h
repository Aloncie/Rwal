#pragma once
#include <iostream>
#include <fstream>
#include <string>

class Logs{
private:
	std::string get_current_time();
public:
	void write_logs(std::string message);
};
