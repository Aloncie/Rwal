#include "logs/logs.hpp"

class MockLogs : public Logs {
public:
    std::vector<std::string> messages;
    void write_logs(const std::string& msg) override {
        messages.push_back(msg);
    }
};
