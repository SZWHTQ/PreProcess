#ifndef JOHNSON_COOK_H
#define JOHNSON_COOK_H

#include "Material.h"

class JohnsonCook : public Material {
   public:
    const double youngsModulus, poissonsRatio;
    const double A, B, n, C, m;
    const double roomTemperature, meltingTemperature;
    const double specificHeatCapacity;
    const double referenceStrainRate;
    const double D1, D2, D3, D4, D5;
    const double failureStrainFactor;
    JohnsonCook(double density_, double youngsModulus_, double poissonsRatio_,
                double A_, double B_, double n_, double C_, double m_,
                double roomTemperature_, double meltingTemperature_,
                double specificHeatCapacity_, double referenceStrainRate_,
                double D1_, double D2_, double D3_, double D4_, double D5_,
                double failureStrainFactor_)
        : Material(density_),
          youngsModulus(youngsModulus_),
          poissonsRatio(poissonsRatio_),
          A(A_),
          B(B_),
          n(n_),
          C(C_),
          m(m_),
          roomTemperature(roomTemperature_),
          meltingTemperature(meltingTemperature_),
          specificHeatCapacity(specificHeatCapacity_),
          referenceStrainRate(referenceStrainRate_),
          D1(D1_),
          D2(D2_),
          D3(D3_),
          D4(D4_),
          D5(D5_),
          failureStrainFactor(failureStrainFactor_){};
    std::string toString() override;
    ~JohnsonCook() override{};
};

#endif