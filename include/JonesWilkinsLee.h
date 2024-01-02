#ifndef JONES_WILKINS_LEE_H
#define JONES_WILKINS_LEE_H

#include "EquationOfState.h"

class Jones_Wilkins_Less : public Equation_of_State {
public:
    const size_t type = 3;
    const std::string name = "Jones-Wilkins-Lee";
    const double A, B, C, R1, R2, omega;
    Jones_Wilkins_Less(size_t id_, double A_, double B_, double C_,
        double R1_, double R2_, double omega_)
        : Equation_of_State(id_)
        , A(A_)
        , B(B_)
        , C(C_)
        , R1(R1_)
        , R2(R2_)
        , omega(omega_) {};
    std::string to_string(size_t count) override;
};

#endif