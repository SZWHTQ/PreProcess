#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include "EquationOfState.h"

class Material {
protected:
    static size_t instanceCount;

public:
    size_t id = 0;
    size_t local_index=0;
    std::string name = "";
    const double density = 0;

    Equation_of_State* eos = nullptr;

    explicit Material(double _density);
    ~Material();

    static size_t getInstanceCount()
    {
        return instanceCount;
    }
    virtual std::string to_string() = 0;
};

#endif