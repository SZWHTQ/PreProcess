#include "Explosive.h"

std::string Explosive::to_string() {
    std::string str = std::to_string(id) + " hiex ";
    str += std::to_string(density) + " ";
    str += std::to_string(detonation_speed);

    return str;
}