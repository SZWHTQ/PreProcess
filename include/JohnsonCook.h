#ifndef JOHNSON_COOK_H
#define JOHNSON_COOK_H

#include "Material.h"

class Johnson_Cook : public Material {
public:
    const double youngs_modulus, poissons_ratio;
    const double A, B, n, C, m;
    const double room_temperature, melting_temperature;
    const double specific_heat_capacity;
    const double reference_strain_rate;
    const double D1, D2, D3, D4, D5;
    const double failure_strain_factor;
    Johnson_Cook(double _density, double _youngs_modulus, double _poissons_ratio,
        double _A, double _B, double _n, double _C, double _m,
        double _room_temperature, double _melting_temperature,
        double _specific_heat_capacity, double _reference_strain_rate,
        double _D1, double _D2, double _D3, double _D4, double _D5,
        double _failure_strain_factor)
        : Material(_density)
        , youngs_modulus(_youngs_modulus)
        , poissons_ratio(_poissons_ratio)
        , A(_A)
        , B(_B)
        , n(_n)
        , C(_C)
        , m(_m)
        , room_temperature(_room_temperature)
        , melting_temperature(_melting_temperature)
        , specific_heat_capacity(_specific_heat_capacity)
        , reference_strain_rate(_reference_strain_rate)
        , D1(_D1)
        , D2(_D2)
        , D3(_D3)
        , D4(_D4)
        , D5(_D5)
        , failure_strain_factor(_failure_strain_factor) {};
    std::string to_string();
};

#endif