#include "funcs.hpp"
#include <random>
#include <unistd.h>
#include <stdexcept>

int random(int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    if (count <= 0) {
        throw std::invalid_argument("Count must be a positive integer");
    }
    std::uniform_int_distribution<> distrib(1, count);
    return distrib(gen);
}
