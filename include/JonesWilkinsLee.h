#ifndef JONES_WILKINS_LEE_H
#define JONES_WILKINS_LEE_H

#include "EquationOfState.h"

class Jones_Wilkins_Less : public Equation_of_State {
public:
    const size_t type = 3;
    const std::string name = "Jones-Wilkins-Lee";
    const double A, B, C, R1, R2, omega;
    Jones_Wilkins_Less(size_t _id, double _A, double _B, double _C,
        double _R1, double _R2, double _omega)
        : Equation_of_State(_id)
        , A(_A)
        , B(_B)
        , C(_C)
        , R1(_R1)
        , R2(_R2)
        , omega(_omega) {};
    std::string to_string();
};

#endif