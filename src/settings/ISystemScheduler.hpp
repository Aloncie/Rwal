#pragma once

#include <string>

namespace rwal::platform:: {

class ISystemScheduler {
public:
	ISystemScheduler() = default;
	virtual ~ISystemScheduler() = default;

	virtual bool status() const;
	virtual bool set(const std::string& value);
	virtual std::string get() const;
	virtual bool reload();
	virtual bool createScheduler();
};
