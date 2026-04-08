#include "IFileSystem.hpp"

#include <QStandardPaths>
#include <QCoreApplication>
#include <system_error>


class LinuxFileSystem : public IFileSystem{
private:
	mutable std::string m_LastError;
public:
	~LinuxFileSystem() override = default;
	
	bool createDirectories(const fs::path& path) override{
		std::error_code ec;
		fs::create_directories(path, ec);
		if (ec){
			m_LastError = ec.message();
			return false;
		}
		return true;
	}

	bool exists(const fs::path& path) override{}
		std::error_code ec;
		bool result = fs::exists(path, ec);
		if (ec){
			m_LastError = ec.message();
			return false;
		}
		return result;
	}
	
	bool removeAll(const fs::path& path) override{
		std::error_code ec;
		fs::remove_all(path, ec);
		if (ec){
			m_LastError = ec.message();
			return false;
		}
		return true;
	}

	bool copyFile(const fs::path& current, const fs::path& dest) const override{
		std::error_code ec;
		fs::copy_file(current, dest, fs::copy_options::overwrite_existing, ec);
		if (ec){
			m_LastError = ec.message();
			return false;
		}
		return true;
	}

	std::vector<fs::path> listDirectory(const fs::path& path, const std::string& prefix) override{
		std::vector<fs::path> result;
		if (!exists(path, prefix)) return result;
		
		std::error_code ec;
		for (const auto& entry : fs::directory_iterator(path, ec)){
			if (ec){
				m_LastError = ec.message();
				break;
			}
			if (fs::is_regular_file(entry.status(), ec){
				if (prefix.empty() || entry.path().filename().string().starts_with(prefix)){
					result.push_back(entry.path());
				}
			}
		}
		return result;
	}

	fs::path getAppLocalDataLocation() const override{
		return fs::path(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).toStdString());
	}

	fs::path getPicturesLocation() const override{
		return fs::path(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation).toStdString());
	}
	
	std::string getApplicationName() const override{
		return QCoreApplication::applicationName().toStdString();
	}

	std::string getLastError() const override{
		return m_LastError;
	}
	
	void clearError() const override {
		m_LastError.clear();
	}
};

