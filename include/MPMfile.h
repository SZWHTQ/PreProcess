#ifndef MPM_FILE_H
#define MPM_FILE_H

#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

#include "gp_Pnt.hxx"

class Model;

struct Double_3D {
    double x = 0, y = 0, z = 0;
};

struct Int_3D {
    int x = 0, y = 0, z = 0;
};

class MPM_File {
private:
    std::string name;
    std::list<std::unique_ptr<Model>> models;
public:

    double dx=0;
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

    Int_3D down_boundary, up_boundary;
    Double_3D down_extend, up_extend;
    Double_3D detonation_point;

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

    MPM_File() = default;
    explicit MPM_File(std::string name_ = "test")
        : name(std::move(name_)) {};
    ~MPM_File() {
        models.clear();
    }

    size_t get_component_num();
    size_t get_particle_num();
    size_t get_material_num();
    std::tuple<gp_Pnt, gp_Pnt> get_max_min_coor();
    void add(const Model& model);
    void write();
};

#endif