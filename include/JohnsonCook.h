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
    Johnson_Cook(double density_, double youngs_modulus_, double poissons_ratio_,
        double A_, double B_, double n_, double C_, double m_,
        double room_temperature_, double melting_temperature_,
        double specific_heat_capacity_, double reference_strain_rate_,
        double D1_, double D2_, double D3_, double D4_, double D5_,
        double failure_strain_factor_)
        : Material(density_)
        , youngs_modulus(youngs_modulus_)
        , poissons_ratio(poissons_ratio_)
        , A(A_)
        , B(B_)
        , n(n_)
        , C(C_)
        , m(m_)
        , room_temperature(room_temperature_)
        , melting_temperature(melting_temperature_)
        , specific_heat_capacity(specific_heat_capacity_)
        , reference_strain_rate(reference_strain_rate_)
        , D1(D1_)
        , D2(D2_)
        , D3(D3_)
        , D4(D4_)
        , D5(D5_)
        , failure_strain_factor(failure_strain_factor_) {};
    std::string to_string() override;
};

#endif