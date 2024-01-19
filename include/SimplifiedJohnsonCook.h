#ifndef SIMPLIFIED_JOHNSON_COOK_H
#define SIMPLIFIED_JOHNSON_COOK_H

#include "Material.h"

class SimplifiedJohnsonCook : public Material {
public:
    const double youngsModulus, poissonsRatio;
    const double A, B, n, C;
    const double referenceStrainRate;
    const double failureStrain;
    SimplifiedJohnsonCook(double density_, double youngsModulus_, double poissonsRatio_,
        double A_, double B_, double n_, double C_, double referenceStrainRate_,
        double failureStrain_)
        : Material(density_)
        , youngsModulus(youngsModulus_)
        , poissonsRatio(poissonsRatio_)
        , A(A_)
        , B(B_)
        , n(n_)
        , C(C_)
        , referenceStrainRate(referenceStrainRate_)
        , failureStrain(failureStrain_) {};
    std::string toString() override;
};

#endif