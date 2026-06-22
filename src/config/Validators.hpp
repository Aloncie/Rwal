#pragma once
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <set>
#include <string>

// Validators must follow division of responsibility rule.
// They can't perform multiple checks at the same time.
//
// Example: nonEmptyArray() can't check type of the array, it must do isArray().

namespace rwal::config::validators {

using json = nlohmann::json;
using Validator = std::function<std::optional<std::string>(const json&)>;

// ====== Type checkers ======
inline std::optional<std::string> isString(const json& j) {
    if (!j.is_string())
        return "must be a string";
    return std::nullopt;
}

inline std::optional<std::string> isBool(const json& j) {
    if (!j.is_boolean())
        return "must be a boolean";
    return std::nullopt;
}

inline std::optional<std::string> isArray(const json& j) {
    if (!j.is_array())
        return "must be an array";
    return std::nullopt;
}

inline std::optional<std::string> isObject(const json& j) {
    if (!j.is_object())
        return "must be an object";
    return std::nullopt;
}

// ====== Emppty checkers (type already checked) ======
inline std::optional<std::string> nonEmptyString(const json& j) {
    if (j.get<std::string>().empty())
        return "string must not be empty";
    return std::nullopt;
}

inline std::optional<std::string> nonEmptyArray(const json& j) {
    if (j.empty())
        return "array must not be empty";
    return std::nullopt;
}

// ====== Valid values checkers ======
inline std::optional<std::string> isValidSorting(const json& j) {
    static const std::set<std::string> valid = {"random", "toplist",   "date_added",
                                                "views",  "favorites", "relewards"};
    if (valid.find(j.get<std::string>()) == valid.end())
        return "invalid sorting method: " + j.get<std::string>();
    return std::nullopt;
}

inline std::optional<std::string> isValidResolution(const json& j) {
    const std::string& s = j.get<std::string>();
    if (s.empty())
        return "resolution must not be empty";
    for (char c : s) {
        if (!isdigit(c) && c != 'x' && c != 'X')
            return "resolution contains invalid characters";
    }
    return std::nullopt;
}

inline std::optional<std::string> isValidUrl(const json& j) {
    const std::string& s = j.get<std::string>();
    if (s.rfind("http://", 0) != 0 && s.rfind("https://", 0) != 0)
        return "URL must start with http:// or https://";
    return std::nullopt;
}

// ====== Exact value checkers ======
inline Validator isExactString(const std::string& expected) {
    return [expected](const json& j) -> std::optional<std::string> {
        if (j.get<std::string>() != expected)
            return "must be exactly '" + expected + "'";
        return std::nullopt;
    };
}

// ====== Wallhaven param_names checker ======
inline std::optional<std::string> isValidWallhavenParamNames(const json& j) {
    static const std::map<std::string, std::string> required = {
        {"apikey", "apikey"},           {"page", "page"}, {"query", "q"}, {"sorting", "sorting"},
        {"resolutions", "resolutions"},
    };

    for (const auto& [key, expected] : required) {
        if (!j.contains(key))
            return "param_names missing required key: " + key;
        if (!j[key].is_string())
            return "param_names." + key + " must be a string";
        if (j[key].get<std::string>() != expected)
            return "param_names." + key + " must be exactly '" + expected + "'";
    }
    return std::nullopt;
}

} // namespace rwal::config::validators
