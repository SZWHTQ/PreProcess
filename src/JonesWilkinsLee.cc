#include "JonesWilkinsLee.h"

std::string JonesWilkinsLee::toString(size_t count)
{
    std::string str = "seos " + std::to_string(count) + " ";
    str += std::to_string(type) + " ";
    str += std::to_string(A) + " ";
    str += std::to_string(B) + " ";
    str += std::to_string(C) + " ";
    str += std::to_string(R1) + " ";
    str += std::to_string(R2) + " ";
    str += std::to_string(omega);

    return str;
}