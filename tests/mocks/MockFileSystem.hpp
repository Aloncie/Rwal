#pragma once
#include "internal/filesystem/IFileSystem.hpp"

#include <gmock/gmock.h>
#include <filesystem>

namespace fs = std::filesystem;

class MockFileSystem : public IFileSystem {
public:
	using IFileSystem::IFileSystem;

	MockFileSystem() = default;
	~MockFileSystem() override = default;

	MOCK_METHOD(bool, exists, (const fs::path&), (const, override));
	MOCK_METHOD(bool, existsDirectory, (const fs::path&), (const, override));
	MOCK_METHOD(bool, createDirectories, (const fs::path&), (override));
	MOCK_METHOD(bool, removeAll, (const fs::path&), (override));
	MOCK_METHOD(bool, copyFile, (const fs::path&, const fs::path&), (const, override));
	MOCK_METHOD(std::vector<fs::path>, listDirectory, (const fs::path&, const std::string&), (const, override));
	MOCK_METHOD(std::optional<uintmax_t>, getFileSize, (const fs::path&), (const, override));
	MOCK_METHOD(bool, remove, (const fs::path&), (override));
	MOCK_METHOD(bool, isRegularFile, (const fs::path&), (const, override));
	MOCK_METHOD(std::optional<fs::file_time_type>, getLastModifiedTime, (const fs::path&), (const, override));
	MOCK_METHOD(bool, rename, (const fs::path&, const fs::path&), (override));

	MOCK_METHOD(fs::path, getAppLocalDataLocation, (), (const, override));
	MOCK_METHOD(fs::path, getPicturesLocation, (), (const, override));
	MOCK_METHOD(fs::path, getTempLocation, (), (const, override));
	MOCK_METHOD(fs::path, getConfigLocation, (), (const, override));
	MOCK_METHOD(fs::path, getSchedulerLocation, (), (const, override));
	MOCK_METHOD(fs::path, getBinaryLocation, (), (const, override));

	MOCK_METHOD(std::string, getLastError, (), (const, override));
	MOCK_METHOD(void, clearError, (), (const, override));
};

