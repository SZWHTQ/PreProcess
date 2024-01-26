#include <sstream>

#include "SimplifiedJohnsonCook.h"

std::string SimplifiedJohnsonCook::toString() {
    std::ostringstream oss;
    oss << localIndex << " sjcf ";
    oss << density << " ";
    oss << youngsModulus << " ";
    oss << poissonsRatio << " ";
    oss << A << " ";
    oss << B << " ";
    oss << n << " ";
    oss << C << " ";
    oss << referenceStrainRate << " ";
    oss << failureStrain;

    return oss.str();
}