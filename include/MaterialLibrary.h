#ifndef MATERIAL_LIBRARY_H
#define MATERIAL_LIBRARY_H

#include <map>

// Material
//  Parent
#include "Material.h"
//  Children
#include "Explosive.h"
#include "JohnsonCook.h"
#include "SimplifiedJohnsonCook.h"

class MaterialLibrary {
   public:
    static JohnsonCook steel;
    static JohnsonCook _2A14T6;
    static JohnsonCook _2A14T4;
    static Explosive RDX;
    static SimplifiedJohnsonCook Pb;

    // Add material to the library
    MaterialLibrary(MaterialLibrary const&) = delete;
    void operator=(MaterialLibrary const&) = delete;
    void add(Material* material);
    std::map<std::string, Material*> get;
    static MaterialLibrary& get_library();

   private:
    MaterialLibrary();

    // Singleton
    static MaterialLibrary library;
};

#endif  // MATERIAL_LIBRARY_H