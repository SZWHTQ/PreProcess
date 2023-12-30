#include "Material.h"

size_t Material::instanceCount = 0;

Material::Material()
    : id(++instanceCount)
{
    name = "Material_" + std::to_string(id);
}

Material::Material(double _density)
    : density(_density)
    , id(++instanceCount)
{
    name = "Material_" + std::to_string(id);
}

Material::~Material()
{
    --instanceCount;
}
