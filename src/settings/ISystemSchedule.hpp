#pragma once

#include <optional>
#include <string>

class ISystemSchedule {
public:
	ISystemSchedule() = default;
	virtual ~ISystemSchedule() = default;
	
	virtual std::string set(const std::string& value) = 0;
	virtual std::string get() const = 0;
protected:
	virtual bool create() = 0;
	virtual bool status() const = 0;
	virtual bool reload() = 0;
	virtual bool start() const = 0;
	virtual bool disable() const = 0;
};

