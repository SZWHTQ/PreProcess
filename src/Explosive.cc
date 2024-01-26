#include "Explosive.h"
#include <sstream>

std::string Explosive::toString() {
    std::ostringstream oss;
    oss << localIndex << " hiex ";
    oss << density << " ";
    oss << detonationSpeed;

    return oss.str();
}