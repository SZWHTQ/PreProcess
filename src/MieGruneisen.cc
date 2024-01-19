#include "MieGruneisen.h"

std::string MieGruneisen::toString(size_t count) {
    std::string str = "seos " + std::to_string(count) + " ";
    str += std::to_string(type) + " ";
    str += std::to_string(C) + " ";
    str += std::to_string(S1) + " ";
    str += std::to_string(S2) + " ";
    str += std::to_string(S3) + " ";
    str += std::to_string(gamma) + " ";
    str += std::to_string(a) + " ";
    str += std::to_string(E0) + " ";
    str += std::to_string(V0);

    return str;
}