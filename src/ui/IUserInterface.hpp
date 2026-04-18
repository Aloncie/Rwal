#pragma once

#include <string>
#include <functional>
#include <optional>
#include <string_view>

class IUserInterface {
public:
    virtual ~IUserInterface() = default;
	virtual void requestInput(std::function<void(std::string)> callback, std::optional<std::string> message = std::nullopt) = 0;
	virtual void showMessage(std::string_view message) = 0;
	virtual bool prepareForExternalCommand() = 0;
	virtual bool refresh() = 0;
};
