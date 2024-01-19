#ifndef MODEL_H
#define MODEL_H

#include <utility>
#include <vector>

#include "TopoDS.hxx"

class Material;

class Model {
   public:
    size_t id;  // component id
    std::string name;
    TopoDS_Shape shape;

    std::vector<gp_Pnt> particles;
    double dx = 0;

    Material* material = nullptr;

    Model(size_t id_, std::string modelName_)
        : id(id_), name(std::move(modelName_)){};
    Model(size_t id_, std::string modelName_, const std::string& filePath_,
          Material* material_);
    ~Model() { particles.clear(); };

    std::tuple<gp_Pnt, gp_Pnt> getMaxMinCoordinates() const;
    bool contain(const gp_Pnt& point) const;
    void fill(const double dx, const bool verbose = false);
    void fillWithOMP(const double dx, const bool verbose = false);
    void fillSequentially(const double dx, const bool verbose = true);
    // void fillMPI(const double dx, const bool verbose = false);
};

#endif
