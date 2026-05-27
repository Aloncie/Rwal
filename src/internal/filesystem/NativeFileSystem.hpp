#pragma once
#include "IFileSystem.hpp"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class NativeFileSystem : public IFileSystem{
private:
	mutable std::string m_LastError;
public:
	NativeFileSystem() = default;
	~NativeFileSystem() override = default;
	
	// Methods with cross-platform implementations, <filesystem> library
	bool exists(const fs::path& path) const override;
	bool existsDirectory(const fs::path& path) const override;
	bool createDirectories(const fs::path& path) override;
	bool removeAll(const fs::path& path) override;
	bool copyFile(const fs::path& current, const fs::path& dest) const override;
	std::vector<fs::path> listDirectory(const fs::path& path, const std::string& prefix = "") const override;
	uintmax_t getFileSize(const fs::path& path) const override;
	bool remove(const fs::path& path) override;
	bool isRegularFile(const fs::path& path) const override;
	fs::file_time_type getLastModifiedTime(const fs::path& path) const override;
	
	// Os specific methods, will be implemented by <OS>FileSystem classes
	fs::path getAppLocalDataLocation() const override = 0;
	fs::path getPicturesLocation() const override = 0;
	fs::path getTempLocation() const override = 0;
	
	// Handle errors
	std::string getLastError() const override;
	void clearError() const override;
};

