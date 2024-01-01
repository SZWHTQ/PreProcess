#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include "EquationOfState.h"

class Material {
protected:
    static size_t instanceCount;

public:
    size_t id;
    size_t local_index;
    std::string name;
    const double density;

    Equation_of_State* eos;

    Material(double _density);
    ~Material();

    static size_t getInstanceCount()
    {
        return instanceCount;
    }
    virtual std::string to_string() = 0;
};

#endif