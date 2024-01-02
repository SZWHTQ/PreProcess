#ifndef EXPLOSIVE_H
#define EXPLOSIVE_H

#include "Material.h"

class Explosive : public Material {
public:
    const double detonation_speed;
    Explosive(double density_, double detonation_speed_)
        : Material(density_)
        , detonation_speed(detonation_speed_) {};
    std::string to_string() override;
};

#endif