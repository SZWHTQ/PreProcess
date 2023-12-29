#include "JonesWilkinsLee.h"

std::string Jones_Wilkins_Less::to_string()
{
    std::string str = "seos " + std::to_string(id) + " ";
    str += std::to_string(type) + " ";
    str += std::to_string(A) + " ";
    str += std::to_string(B) + " ";
    str += std::to_string(C) + " ";
    str += std::to_string(R1) + " ";
    str += std::to_string(R2) + " ";
    str += std::to_string(omega);

    return str;
}