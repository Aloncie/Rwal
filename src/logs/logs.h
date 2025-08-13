#include <memory>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>

class Logs{
public:
	std::string get_current_time();
	void write_logs(std::string message);
};
