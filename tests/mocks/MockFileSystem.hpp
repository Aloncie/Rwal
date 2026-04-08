#pragma once
#include "internal/filesystem/IFileSystem.hpp"

#include <gmock/gmock.h>
#include <filesystem>

namespace fs = std::filesystem;

class MockFileSystem : public IFileSystem {
public:
	MOCK_METHOD(bool, exists, (const fs::path& path), (override));
	MOCK_METHOD(bool, createDirectories, (const fs::path& path), (override));
	MOCK_METHOD(bool, removeAll, (const fs::path& path), (override));
	MOCK_METHOD(bool, copyFile, (const fs::path& source, const fs::path& destination), (override));
	MOCK_METHOD(std::vector<fs::path>, listDirectory, (const fs::path& path, const std::string& prefix), (override));
	MOCK_METHOD(fs::path, getAppLocalDataLocation, (), (const, override));
	MOCK_METHOD(fs::path, getPicturesLocation, (), (const, override));
	MOCK_METHOD(std::string, getApplicationName, (), (const, override));
	MOCK_METHOD(std::string, getLastError, (), (const, override));
	MOCK_METHOD(void, clearError, (), (const, override));
};

