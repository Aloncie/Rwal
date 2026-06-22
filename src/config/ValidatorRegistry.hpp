#pragma once
#include "Validators.hpp"

#include <map>
#include <optional>
#include <string>
#include <vector>

class ValidatorRegistry {
public:
    using Validator = rwal::config::validators::Validator;

    void add(const std::string& path, std::vector<Validator> validators) {
        registry_[path] = std::move(validators);
    }

    std::optional<std::string>
    validate(const std::string& path, const nlohmann::json& value) const {
        auto it = registry_.find(path);
        if (it == registry_.end())
            return std::nullopt; // not rules

        for (const auto& validator : it->second) {
            auto error = validator(value);
            if (error)
                return error;
        }
        return std::nullopt;
    }

private:
    std::map<std::string, std::vector<Validator>> registry_;
};
