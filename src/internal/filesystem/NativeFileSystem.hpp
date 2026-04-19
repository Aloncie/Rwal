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

	bool exists(const fs::path& path) const override;
	bool createDirectories(const fs::path& path) override;
	bool removeAll(const fs::path& path) override;
	bool copyFile(const fs::path& current, const fs::path& dest) const override;
	std::vector<fs::path> listDirectory(const fs::path& path, const std::string& prefix = "") const override;

	fs::path getAppLocalDataLocation() const override;
	fs::path getPicturesLocation() const override;
	std::string getApplicationName() const override;
	
	std::string getLastError() const override;
	void clearError() const override;
};
