#include "Explosive.h"

std::string Explosive::to_string(size_t count) {
    std::string str = std::to_string(count) + " hiex ";
    str += std::to_string(density) + " ";
    str += std::to_string(detonation_speed);

    return str;
}