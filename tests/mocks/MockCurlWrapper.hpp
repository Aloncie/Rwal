#pragma once
#include "net/CurlWrapper.hpp"

#include <gmock/gmock.h>
#include <string>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

class MockCurlWrapper : public CurlWrapper {
public:
	using CurlWrapper::CurlWrapper;
	explicit MockCurlWrapper(Logs& logs) : CurlWrapper(logs) {}

	~MockCurlWrapper() override = default;

	MOCK_METHOD(void, getRequest, (const std::string& url), (override));
	MOCK_METHOD(std::string, getData, (const std::string& paragraph, const std::string& str), (override));
	MOCK_METHOD(std::optional<fs::path>, downloadImage, (const std::string& image_url), (override));

};

