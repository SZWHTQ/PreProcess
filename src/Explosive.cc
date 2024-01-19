#include "Explosive.h"

std::string Explosive::toString() {
    std::string str = std::to_string(localIndex) + " hiex ";
    str += std::to_string(density) + " ";
    str += std::to_string(detonationSpeed);

    return str;
}