set(dir "${CMAKE_CURRENT_SOURCE_DIR}")

list(
    APPEND srcs
    "${dir}/EquationOfState.cc"
    "${dir}/Explosive.cc"
    "${dir}/JohnsonCook.cc"
    "${dir}/JonesWilkinsLee.cc"
    "${dir}/Material.cc"
    "${dir}/MaterialLibrary.cc"
    "${dir}/MieGruneisen.cc"
    "${dir}/Model.cc"
    "${dir}/MpmFile.cc"
    "${dir}/SimplifiedJohnsonCook.cc"
    "${dir}/Timer.cc"
)

set(srcs "${srcs}" PARENT_SCOPE)