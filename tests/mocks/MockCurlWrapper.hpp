#pragma once
#include "internal/filesystem/IFileSystem.hpp"
#include "net/CurlWrapper.hpp"

#include <filesystem>
#include <gmock/gmock.h>
#include <optional>
#include <string>

namespace fs = std::filesystem;

class MockCurlWrapper : public CurlWrapper {
public:
    using CurlWrapper::CurlWrapper;
    MockCurlWrapper(Logs& logs, IFileSystem& fs) : CurlWrapper(logs, fs) {}

    ~MockCurlWrapper() override = default;

    MOCK_METHOD(void, getRequest, (const std::string& url), (override));
    MOCK_METHOD(
        std::string, getData, (const std::string& paragraph, const std::string& str), (override));
    MOCK_METHOD(std::optional<fs::path>, downloadImage, (const std::string& image_url), (override));
    MOCK_METHOD(void, clearning, (), (override));
    MOCK_METHOD(void, generateUniqueSuffix, (std::string & filename), (override));
    MOCK_METHOD(std::string, getFilenameFromUrl, (const std::string& image_url), (override));
};
