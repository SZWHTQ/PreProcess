#ifndef MPM_FILE_H
#define MPM_FILE_H

#include <list>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

#include "gp_Pnt.hxx"

class Model;

struct Double3D {
    double x = 0, y = 0, z = 0;
};

struct Int3D {
    int x = 0, y = 0, z = 0;
};

class MpmFile {
   private:
    std::string name;
    std::list<std::unique_ptr<Model>> models;

   public:
    double dx = 0;
    double dCell_scale = 1.8;
    double dt_scale = 0.1;
    double end_time = 0;
    double out_time = 0;
    double rpt_time = 0;

    size_t component_num = 0;
    size_t particle_num = 0;

    enum class UNIT {
        MMGS_ms,
    } unit = UNIT::MMGS_ms;

    enum class ALGORITHM {
        MUSL,
        USF,
        USL,
    } algorithm = ALGORITHM::MUSL;

    // enum class POST_PROCESS {
    //     PARAVIEW,
    //     TECPLOT,
    //     UNV,
    // };
    struct Post_Process {
        bool paraview = true;
        bool tecplot = false;
        bool unv = false;
    } post_process;

    enum class BOUNDARY {
        FREE,
        FIX,
        SYMMETRY,
    };

    Int3D down_boundary, up_boundary;
    Double3D down_extend, up_extend;
    Double3D detonation_point;

    bool GIMP = true;
    bool Jaumann = true;

    bool Contact = false;
    enum class CONTACT {
        LAGRANGIAN,
        PENALTY,
    };
    struct Contact_Parameters {
        CONTACT type = CONTACT::LAGRANGIAN;
        double friction = 0;
        double body_norm = 2;
    } contact_parameters;

    MpmFile() = default;
    explicit MpmFile(std::string name_ = "test") : name(std::move(name_)){};
    ~MpmFile() { models.clear(); }

    size_t getComponentNumber();
    size_t getParticleNumber();
    size_t getMaterialNumber();
    std::tuple<gp_Pnt, gp_Pnt> getMaxMinCoordinates();
    void add(const Model& model);
    void write();
};

#endif