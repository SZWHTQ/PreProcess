#include "SimplifiedJohnsonCook.h"

std::string SimplifiedJohnsonCook::toString() {
    std::string str = std::to_string(localIndex) + " sjcf ";
    str += std::to_string(density) + " ";
    str += std::to_string(youngsModulus) + " ";
    str += std::to_string(poissonsRatio) + " ";
    str += std::to_string(A) + " ";
    str += std::to_string(B) + " ";
    str += std::to_string(n) + " ";
    str += std::to_string(C) + " ";
    str += std::to_string(referenceStrainRate) + " ";
    str += std::to_string(failureStrain);

    return str;
}