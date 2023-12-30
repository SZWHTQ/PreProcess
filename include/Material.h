#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include "EquationOfState.h"

class Material {
protected:
    static size_t instanceCount;

public:
    size_t id;
    std::string name;
    double density;
    Equation_of_State* eos;
    Material();
    Material(double _density);
    ~Material();

    static size_t getInstanceCount()
    {
        return instanceCount;
    }
    virtual std::string to_string(size_t count) = 0;
};

#endif