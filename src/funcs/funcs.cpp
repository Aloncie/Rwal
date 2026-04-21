#include "funcs.hpp"
#include <random>
#include <stdexcept>
#include <ctime>

int random(int count) {
    if (count <= 0) {
        throw std::invalid_argument("Count must be a positive integer");
    }
	thread_local std::mt19937 gen([]{
		std::random_device rd;
		return std::mt19937(rd() ^ (std::time(nullptr) << 7));
	}());
    return std::uniform_int_distribution<>(1, count)(gen);
}

