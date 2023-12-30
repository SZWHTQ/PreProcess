#ifndef EXPLOSIVE_H
#define EXPLOSIVE_H

#include "Material.h"

class Explosive : public Material {
public:
    const double detonation_speed;
    Explosive(double _density, double _detonation_speed)
        : Material(_density)
        , detonation_speed(_detonation_speed) {};
    std::string to_string(size_t count);
};

#endif