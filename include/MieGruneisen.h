#ifndef MIE_GRUNEISEN_H
#define MIE_GRUNEISEN_H

#include "EquationOfState.h"

class MieGruneisen : public EquationOfState {
   public:
    const size_t type = 4;
    const std::string name = "Mie Gruneisen";
    const double C, S1, S2, S3, gamma, a, E0, V0;
    MieGruneisen(size_t id_, double C_, double S1_, double S2_, double S3_,
                 double gamma_, double a_ = 0, double E0_ = 0, double V0_ = 0)
        : EquationOfState(id_),
          C(C_),
          S1(S1_),
          S2(S2_),
          S3(S3_),
          gamma(gamma_),
          a(a_),
          E0(E0_),
          V0(V0_){};
    std::string toString(size_t count) override;
};

#endif