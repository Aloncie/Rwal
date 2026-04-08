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
	~MockCurlWrapper() override = default;

	MOCK_METHOD(void, getRequest, (std::string url), (override));
	MOCK_METHOD(std::string, getData, (std::string paragraph, std::string str), (override));
	MOCK_METHOD(std::optional<std::filesystem::path>, downloadImage, (std::string image_url), (override));

};

