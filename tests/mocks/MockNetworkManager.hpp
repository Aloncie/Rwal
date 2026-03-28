#pragma once
#include <gmock/gmock.h>
#include "net/NetworkManager.hpp"

class MockNetworkManager : public NetworkManager {
public:
    MOCK_METHOD(std::optional<std::filesystem::path>, fetchImage, (std::string keyword),(override));
    MOCK_METHOD(bool, isAvailable, (), (override));
    MOCK_METHOD(std::string, craftUrl, (std::string keyword, std::optional<std::string> page),(override));

    // Helper to set up successful fetch
    void setSuccessfulFetch(fs::path path) {
        ON_CALL(*this, isAvailable()).WillByDefault(Return(true));
        ON_CALL(*this, fetchImage(testing::_)).WillByDefault(Return(path));
    }

    // Helper to set up failed fetch
    void setFailedFetch() {
        ON_CALL(*this, isAvailable()).WillByDefault(Return(true));
        ON_CALL(*this, fetchImage(testing::_)).WillByDefault(Return(std::nullopt));
    }

    // Helper to set up offline mode
    void setOffline() { ON_CALL(*this, isAvailable()).WillByDefault(Return(false)); }
};
