#pragma once

#include <string>

class ISystemScheduler {
public:
	ISystemScheduler() = default;
	virtual ~ISystemScheduler() = default;
	
	virtual std::string set(const std::string& value);
	virtual std::string get() const;
protected:
	virtual bool create();
	virtual bool status() const;
	virtual bool reload() const;
	virtual bool start() const;
	virtual bool disable() const;
};

