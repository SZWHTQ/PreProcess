#include <sstream>

#include "MieGruneisen.h"

std::string MieGruneisen::toString(size_t count) {
    std::ostringstream oss;
    oss << "seos " << count << " ";
    oss << type << " ";
    oss << C << " ";
    oss << S1 << " ";
    oss << S2 << " ";
    oss << S3 << " ";
    oss << gamma << " ";
    oss << a << " ";
    oss << E0 << " ";
    oss << V0;

    return oss.str();
}