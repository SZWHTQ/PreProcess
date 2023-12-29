#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include "EquationOfState.h"

class Material {
protected:
    static size_t instanceCount;

public:
    size_t id;
    double density;
    Equation_of_State* eos;
    Material()
        : id(++instanceCount) {};
    Material(double _density)
        : density(_density)
        , id(++instanceCount) {};
    ~Material()
    {
        --instanceCount;
    };
    static size_t getInstanceCount()
    {
        return instanceCount;
    }
    virtual std::string to_string() = 0;
};

#endif