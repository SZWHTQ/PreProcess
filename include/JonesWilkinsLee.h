#ifndef JONES_WILKINS_LEE_H
#define JONES_WILKINS_LEE_H

#include "EquationOfState.h"

class JonesWilkinsLee : public EquationOfState {
public:
    const size_t type = 3;
    const std::string name = "Jones Wilkins Lee";
    const double A, B, C, R1, R2, omega;
    JonesWilkinsLee(size_t id_, double A_, double B_, double C_,
        double R1_, double R2_, double omega_)
        : EquationOfState(id_)
        , A(A_)
        , B(B_)
        , C(C_)
        , R1(R1_)
        , R2(R2_)
        , omega(omega_) {};
    std::string toString(size_t count) override;
};

#endif