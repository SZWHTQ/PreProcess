#ifndef SIMPLIFIED_JOHNSON_COOK_H
#define SIMPLIFIED_JOHNSON_COOK_H

#include "Material.h"

class Simplified_Johnson_Cook : public Material {
public:
    const double youngs_modulus, poissons_ratio;
    const double A, B, n, C;
    const double reference_strain_rate;
    const double failure_strain;
    Simplified_Johnson_Cook(double density_, double youngs_modulus_, double poissons_ratio_,
        double A_, double B_, double n_, double C_, double reference_strain_rate_,
        double failure_strain_)
        : Material(density_)
        , youngs_modulus(youngs_modulus_)
        , poissons_ratio(poissons_ratio_)
        , A(A_)
        , B(B_)
        , n(n_)
        , C(C_)
        , reference_strain_rate(reference_strain_rate_)
        , failure_strain(failure_strain_) {};
    std::string to_string() override;
};

#endif