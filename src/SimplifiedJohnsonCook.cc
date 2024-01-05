#include "SimplifiedJohnsonCook.h"

std::string Simplified_Johnson_Cook::to_string() {
    std::string str = std::to_string(local_index) + " sjcf ";
    str += std::to_string(density) + " ";
    str += std::to_string(youngs_modulus) + " ";
    str += std::to_string(poissons_ratio) + " ";
    str += std::to_string(A) + " ";
    str += std::to_string(B) + " ";
    str += std::to_string(n) + " ";
    str += std::to_string(C) + " ";
    str += std::to_string(reference_strain_rate) + " ";
    str += std::to_string(failure_strain);

    return str;
}