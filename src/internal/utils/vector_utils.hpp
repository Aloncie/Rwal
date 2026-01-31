#pragma once
#include <vector>
#include <algorithm>

namespace rwal::utils::vector {
    template<typename T>
    inline void add_unique(std::vector<T>& v, const T& element) {
        if (std::find(v.begin(), v.end(), element) == v.end()) {
            v.push_back(element);
        }
    }
}
