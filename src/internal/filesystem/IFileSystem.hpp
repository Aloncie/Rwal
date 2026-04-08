#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * Abstract interface for file system operations.
 * Allows mocking for unit tests.
 */

class IFileSystem{
public:
	virtual ~IFileSystem() = default;

	// File operations
	virtual bool exists(const fs::path& path) const = 0;
	virtual bool createDirectories(const fs::path& path) = 0;
	virtual bool removeAll(const fs::path& path) = 0;
	virtual std::vector<fs::path> listDirectory(const fs::path& path, const std::string& prefix = "") const = 0;

	// Qt path resolution
	
	virtual fs::path getAppLocalDataLocation() const = 0;
	virtual fs::path getPicturesLocation() const = 0;
	virtual std::string getApplicationName() const = 0;

	// Error handling
	
	virtual std::string getLastError() const = 0;
	virtual void clearError() const = 0;
}
