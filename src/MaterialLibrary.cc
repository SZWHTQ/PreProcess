#include "MaterialLibrary.h"

Johnson_Cook Material_Library::steel(
    7.85e-3,
    2e5,
    0.28,
    525,
    101,
    0.081,
    0.1739,
    1.635,
    293,
    1800,
    47.7,
    1e-6,
    0.0705,
    1.732,
    -0.54,
    -0.0123,
    0,
    1);

Johnson_Cook Material_Library::_2A14T6(
    2.8e-3,
    6.89e4,
    0.33,
    402,
    635,
    0.9,
    0.0075,
    1.426,
    293,
    863,
    8.8e2,
    1e-6,
    0.116,
    0.211,
    -2.172,
    0.012,
    -0.01256,
    1);

Johnson_Cook Material_Library::_2A14T4(
    2.8e-3,
    6.89e4,
    0.33,
    295,
    635,
    0.9,
    0.0075,
    1.429,
    293,
    863,
    8.8e2,
    1e-6,
    0.116,
    0.211,
    -2.172,
    0.012,
    -0.01256,
    1.1);

Material_Library Material_Library::library;

Material_Library::Material_Library()
{
    steel.eos = new Mie_Gruneisen(steel.id,
        4569,
        1.49,
        0,
        0,
        2.1700001,
        0.46);
    steel.name = "steel";
    add(&steel);

    _2A14T6.eos = new Mie_Gruneisen(_2A14T6.id,
        4080,
        1.86,
        0,
        0,
        2.7720001);
    _2A14T6.name = "2A14T6";
    add(&_2A14T6);

    _2A14T4.eos = new Mie_Gruneisen(_2A14T4.id,
        4080,
        1.86,
        0,
        0,
        2.7720001);
    _2A14T4.name = "2A14T4";
    add(&_2A14T4);
}

Material_Library& Material_Library::get_library()
{
    return library;
}

void Material_Library::add(Material* material)
{
    get[material->name] = material;
}