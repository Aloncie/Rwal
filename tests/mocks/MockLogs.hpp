#pragma once
#include <gmock/gmock.h>
#include "logs/logs.hpp"

class MockLogs : public Logs {
public:
    MOCK_METHOD(void, writeLogs, (std::string_view msg), (override));
};

