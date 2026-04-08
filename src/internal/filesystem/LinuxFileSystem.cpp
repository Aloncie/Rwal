#include "LinuxFileSystem.hpp"

#include <QStandardPaths>
#include <QCoreApplication>
#include <system_error>

bool LinuxFileSystem::createDirectories(const fs::path& path) {
	std::error_code ec;
	fs::create_directories(path, ec);
	if (ec) {
		m_LastError = "Failed to create directories: " + ec.message();
		return false;
	}
	return true;
}

bool LinuxFileSystem::exists(const fs::path& path) const {
	std::error_code ec;
	bool result = fs::exists(path, ec);
	if (ec) {
		m_LastError = "Failed to check existence: " + ec.message();
		return false;
	}
	return result;
}

bool LinuxFileSystem::removeAll(const fs::path& path) {
	std::error_code ec;
	fs::remove_all(path, ec);
	if (ec) {
		m_LastError = "Failed to remove: " + ec.message();
		return false;
	}
	return true;
}

bool LinuxFileSystem::copyFile(const fs::path& current, const fs::path& dest) const {
	std::error_code ec;
	fs::copy_file(current, dest, fs::copy_options::overwrite_existing, ec);
	if (ec) {
		m_LastError = "Failed to copy file: " + ec.message();
		return false;
	}
	return true;
}

std::vector<fs::path> LinuxFileSystem::listDirectory(const fs::path& path, const std::string& prefix) const {
	std::vector<fs::path> result;
	std::error_code ec;
	for (const auto& entry : fs::directory_iterator(path, ec)) {
		if (ec) {
			m_LastError = "Failed to list directory: " + ec.message();
			return {};
		}
		if (entry.is_regular_file() && entry.path().filename().string().starts_with(prefix) == 0) {
			result.push_back(entry.path());
		}
	}
	return result;
}

fs::path LinuxFileSystem::getAppLocalDataLocation() const {
	return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).toStdString();
}

fs::path LinuxFileSystem::getPicturesLocation() const {
	return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation).toStdString();

}
std::string LinuxFileSystem::getApplicationName() const {
	return QCoreApplication::applicationName().toStdString();
}

std::string LinuxFileSystem::getLastError() const {
	return m_LastError;
}

void LinuxFileSystem::clearError() const {
	m_LastError.clear();
}
