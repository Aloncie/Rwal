#pragma once

#include <optional>
#include <string>

class ISystemScheduler {
public:
	ISystemScheduler() = default;
	virtual ~ISystemScheduler() = default;
	// return string for answer user, get value	
	virtual std::string set(const std::string& value) = 0;
	virtual std::string get() const = 0;
protected:
	// another methods can be bool only for answer:
	// "How did you do this operation? Success or failure?"
	virtual bool create() = 0;
	virtual bool status() const = 0;
	virtual bool reload() = 0;
	virtual bool start() const = 0;
	virtual bool disable() const = 0;
};

