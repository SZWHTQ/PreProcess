#include <sstream>

#include "JonesWilkinsLee.h"

std::string JonesWilkinsLee::toString(size_t count) {
    std::ostringstream oss;
    oss << "seos " << count << " ";
    oss << type << " ";
    oss << A << " ";
    oss << B << " ";
    oss << C << " ";
    oss << R1 << " ";
    oss << R2 << " ";
    oss << omega;

    return oss.str();
}