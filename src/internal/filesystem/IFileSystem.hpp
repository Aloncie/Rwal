#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

/**
 * Abstract interface for file system operations.
 * Allows mocking for unit tests.
 */

class IFileSystem{
public:
	virtual ~IFileSystem() = default;

	// Basic operations
	virtual bool exists(const fs::path& path) const = 0;
	virtual bool existsDirectory(const fs::path& path) const = 0;
	virtual bool createDirectories(const fs::path& path) = 0;
	virtual bool removeAll(const fs::path& path) = 0;
	virtual bool copyFile(const fs::path& current, const fs::path& dest) const = 0;
	virtual std::vector<fs::path> listDirectory(const fs::path& path, const std::string& prefix = "") const = 0;
	virtual uintmax_t getFileSize(const fs::path& path) const = 0;
	virtual bool remove(const fs::path& path) = 0;
	virtual bool isRegularFile(const fs::path& path) const = 0;
	virtual fs::file_time_type getLastModifiedTime(const fs::path& path) const = 0;

	// Qt path resolution
	virtual fs::path getAppLocalDataLocation() const = 0;
	virtual fs::path getPicturesLocation() const = 0;
	virtual fs::path getTempLocation() const = 0;

	// Error handling
	virtual std::string getLastError() const = 0;
	virtual void clearError() const = 0;
};

