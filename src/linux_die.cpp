#include <iostream>

#include "die.hpp"

void die(const char *reason) {
    std::cerr << reason << std::endl;
    exit(-1);
}