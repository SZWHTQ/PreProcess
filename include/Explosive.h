#ifndef EXPLOSIVE_H
#define EXPLOSIVE_H

#include "Material.h"

class Explosive : public Material {
public:
    const double detonationSpeed;
    Explosive(double density_, double detonationSpeed_)
        : Material(density_)
        , detonationSpeed(detonationSpeed_) {};
    std::string toString() override;
    ~Explosive() override{};
};

#endif