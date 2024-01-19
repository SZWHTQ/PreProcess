#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include "EquationOfState.h"

class Material {
   protected:
    static size_t instanceCount;

   public:
    size_t id = 0;
    size_t localIndex = 0;
    std::string name = "";
    const double density = 0;

    EquationOfState* eos = nullptr;

    explicit Material(double _density);
    ~Material();

    static size_t getInstanceCount() { return instanceCount; }
    virtual std::string toString() = 0;
};

#endif