#ifndef MATERIAL_LIBRARY_H
#define MATERIAL_LIBRARY_H

#include <map>

// Material
//  Parent
#include "Material.h"
//  Children
#include "Explosive.h"
#include "JohnsonCook.h"

// Equation of State
//  Parent
#include "EquationOfState.h"
//  Children
#include "JonesWilkinsLee.h"
#include "MieGruneisen.h"

class Material_Library {
public:
    static Johnson_Cook steel;
    static Johnson_Cook _2A14T6;
    static Johnson_Cook _2A14T4;
    static Explosive RDX;

    // Add material to the library
    Material_Library(Material_Library const&) = delete;
    void operator=(Material_Library const&) = delete;
    void add(Material* material);
    std::map<std::string, Material*> get;
    static Material_Library& get_library();

private:
    Material_Library();

    // Singleton
    static Material_Library library;

};

#endif // MATERIAL_LIBRARY_H