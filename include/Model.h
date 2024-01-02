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
    double dx=0;

    Material* material;

    Model(size_t _id, std::string _model_name)
        : id(_id)
        , name(std::move(_model_name)){};
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
    bool contain(gp_Pnt* point) const;
    void fill_with_particle(double dx, bool verbose=true);
    void fill_with_particle_omp(double dx, bool verbose=true);
    void fill_with_particle_parallel(double dx, bool verbose=true);
};

#endif