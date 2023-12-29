#ifndef MIE_GRUNEISEN_H
#define MIE_GRUNEISEN_H

#include "EquationOfState.h"

class Mie_Gruneisen : public Equation_of_State {
public:
    const size_t type = 4;
    const std::string name = "Mie-Gruneisen";
    const double C, S1, S2, S3, gamma, a, E0, V0;
    Mie_Gruneisen(size_t _id, double _C,
        double _S1, double _S2, double _S3,
        double _gamma, double _a = 0,
        double _E0 = 0, double _V0 = 0)
        : Equation_of_State(_id)
        , C(_C)
        , S1(_S1)
        , S2(_S2)
        , S3(_S3)
        , gamma(_gamma)
        , a(_a)
        , E0(_E0)
        , V0(_V0) {};
    std::string to_string();
};

#endif