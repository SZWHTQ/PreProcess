#include "JohnsonCook.h"

std::string Johnson_Cook::to_string() {
    std::string str = std::to_string(id) + " jcst ";
    str += std::to_string(density) + " ";
    str += std::to_string(youngs_modulus) + " ";
    str += std::to_string(poissons_ratio) + " ";
    str += std::to_string(A) + " ";
    str += std::to_string(B) + " ";
    str += std::to_string(n) + " ";
    str += std::to_string(C) + " ";
    str += std::to_string(m) + " ";
    str += std::to_string(room_temperature) + " ";
    str += std::to_string(melting_temperature) + " ";
    str += std::to_string(specific_heat_capacity) + " ";
    str += std::to_string(reference_strain_rate) + " ";
    str += std::to_string(D1) + " ";
    str += std::to_string(D2) + " ";
    str += std::to_string(D3) + " ";
    str += std::to_string(D4) + " ";
    str += std::to_string(D5) + " ";
    str += std::to_string(failure_strain_factor);

    return str;
}