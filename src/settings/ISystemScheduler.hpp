#pragma once

#include <optional>
#include <string>

class ISystemScheduler {
public:
	ISystemScheduler() = default;
	virtual ~ISystemScheduler() = default;
	
	virtual std::string set(const std::string& value) = 0;
	virtual std::string get() const = 0;
protected:
	virtual bool create() = 0;
	virtual std::optional<bool> status() const = 0;
	virtual bool reload() const = 0;
	virtual bool start() const = 0;
	virtual bool disable() const = 0;
};

