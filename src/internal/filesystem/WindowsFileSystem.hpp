#pragma once

#include "NativeFileSystem.hpp"

namespace fs = std::filesystem;

class WindowsFileSystem : public NativeFileSystem {
public:
	fs::path getAppLocalDataLocation() const override;
    fs::path getPicturesLocation() const override;
	fs::path getTempLocation() const override;
};

