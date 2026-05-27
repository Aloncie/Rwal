#include "NativeFileSystem.hpp"

#include <system_error>

bool NativeFileSystem::createDirectories(const fs::path& path) {
	std::error_code ec;
	fs::create_directories(path, ec);
	if (ec) {
		m_LastError = "Failed to create directories: " + ec.message();
		return false;
	}
	return true;
}

bool NativeFileSystem::exists(const fs::path& path) const {
	std::error_code ec;
	bool result = fs::exists(path, ec);
	if (ec) {
		m_LastError = "Failed to check existence: " + ec.message();
		return false;
	}
	return result;
}

bool NativeFileSystem::existsDirectory(const fs::path& path) const {
	std::error_code ec;
	if (fs::is_directory(path, ec)){
		return true;
	}
	if (ec) {
		m_LastError = "Failed to check directory existence: " + ec.message();
	}
	return false;
}
bool NativeFileSystem::removeAll(const fs::path& path) {
	std::error_code ec;
	fs::remove_all(path, ec);
	if (ec) {
		m_LastError = "Failed to recursively remove: " + ec.message();
		return false; 
	}
	return true;
}

bool NativeFileSystem::remove(const fs::path& path) {
	std::error_code ec;
	fs::remove(path, ec);
    if (ec) {
        m_LastError = "Failed to remove: " + ec.message();
        return false; 
	}
    return true;
}

bool NativeFileSystem::isRegularFile(const fs::path& path) const {
	std::error_code ec;
	bool result = fs::is_regular_file(path, ec);
	if (ec) {
		m_LastError = "Failed to check regular file status: " + ec.message();
		return false;
	}
	return result;
}

bool NativeFileSystem::copyFile(const fs::path& current, const fs::path& dest) const {
	std::error_code ec;
	fs::copy_file(current, dest, fs::copy_options::overwrite_existing, ec);
	if (ec) {
		m_LastError = "Failed to copy file: " + ec.message();
		return false;
	}
	return true;
}

std::vector<fs::path> NativeFileSystem::listDirectory(const fs::path& path, const std::string& prefix) const {
    std::vector<fs::path> result;
    std::error_code ec;
    
    // Initialize the iterator safely
    auto it = fs::directory_iterator(path, ec);
    if (ec) {
        m_LastError = "Failed to open directory: " + ec.message();
        return {};
    }

    // Loop manually using the safe end-iterator comparison
    while (it != fs::directory_iterator()) {
        const auto& entry = *it;

        if (isRegularFile(entry.path) && entry.path().filename().string().starts_with(prefix)) {
            result.push_back(entry.path());
        }

        it.increment(ec);
        if (ec) {
            m_LastError = "Failed to read next directory entry: " + ec.message();
            return {};
        }
    }

    return result;
}

uintmax_t NativeFileSystem::getFileSize(const fs::path& path) const {
    std::error_code ec;
    return fs::file_size(path, ec);
}

fs::file_time_type NativeFileSystem::getLastModifiedTime(const fs::path& path) const {
	std::error_code ec;

	auto time = fs::last_write_time(path, ec);
	if (ec){
		m_LastError = "Failed to get last modified time: " + ec.message();
        return 0;
	}
	return time;
}

std::string NativeFileSystem::getLastError() const {
	return m_LastError;
}

void NativeFileSystem::clearError() const {
	m_LastError.clear();
}
