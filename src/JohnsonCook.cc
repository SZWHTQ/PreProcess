#include <sstream>

#include "JohnsonCook.h"

std::string JohnsonCook::toString() {
    std::ostringstream oss;
    oss << localIndex << " jcst ";
    oss << density << " ";
    oss << youngsModulus << " ";
    oss << poissonsRatio << " ";
    oss << A << " ";
    oss << B << " ";
    oss << n << " ";
    oss << C << " ";
    oss << m << " ";
    oss << roomTemperature << " ";
    oss << meltingTemperature << " ";
    oss << specificHeatCapacity << " ";
    oss << referenceStrainRate << " ";
    oss << D1 << " ";
    oss << D2 << " ";
    oss << D3 << " ";
    oss << D4 << " ";
    oss << D5;

    return oss.str();
}