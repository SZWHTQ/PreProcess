#include "JohnsonCook.h"

std::string JohnsonCook::toString() {
    std::string str = std::to_string(localIndex) + " jcst ";
    str += std::to_string(density) + " ";
    str += std::to_string(youngsModulus) + " ";
    str += std::to_string(poissonsRatio) + " ";
    str += std::to_string(A) + " ";
    str += std::to_string(B) + " ";
    str += std::to_string(n) + " ";
    str += std::to_string(C) + " ";
    str += std::to_string(m) + " ";
    str += std::to_string(roomTemperature) + " ";
    str += std::to_string(meltingTemperature) + " ";
    str += std::to_string(specificHeatCapacity) + " ";
    str += std::to_string(referenceStrainRate) + " ";
    str += std::to_string(D1) + " ";
    str += std::to_string(D2) + " ";
    str += std::to_string(D3) + " ";
    str += std::to_string(D4) + " ";
    str += std::to_string(D5) + " ";

    return str;
}