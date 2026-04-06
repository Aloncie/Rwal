#pragma once
#include "IWallpaperSetter.hpp"
#include "logs/logs.hpp"

#include <memory>

class WallpaperFactory{
private:
	Logs& m_logs;
public:
	explicit WallpaperFactory(Logs& logs) : m_logs(logs) {};
	std::unique_ptr<IWallpaperSetter> create();
};

