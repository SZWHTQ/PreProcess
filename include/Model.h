#ifndef MODEL_H
#define MODEL_H

#include <utility>
#include <vector>

#include "TopoDS.hxx"

class Material;

class Model {
public:
    size_t id; // component id
    std::string name;
    TopoDS_Shape shape;

    std::vector<gp_Pnt> particles;
    double dx = 0;

    Material* material = nullptr;

    Model(size_t _id, std::string _model_name)
        : id(_id)
        , name(std::move(_model_name)) {};
    Model(
        size_t _id,
        std::string _model_name,
        const std::string& _file_path,
        Material* _material);
    ~Model()
    {
        particles.clear();
    };

    std::tuple<gp_Pnt, gp_Pnt> get_max_min_coor() const;
    bool contain(const gp_Pnt& point) const;
    void fill_with_particle(const double dx, const bool verbose = true);
    void fill_with_particle_omp(const double dx, const bool verbose = true);
    void fill_with_particle_parallel(const double dx, const bool verbose = false);
    // void fill_with_particle_mpi(const double dx, const bool verbose = false);
};

#endif