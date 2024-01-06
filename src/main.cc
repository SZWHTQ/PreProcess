// Usage Example

#include <cmath>
#include <iostream>
#include <vector>

#include "BRepAlgoAPI_Cut.hxx"
#include "BRepPrimAPI_MakeCylinder.hxx"

#include "Explosive.h"
#include "JonesWilkinsLee.h"
#include "MPMfile.h"
#include "MaterialLibrary.h"
#include "Model.h"

const TopoDS_Shape make_cylinder(double radius, double height, gp_Pnt center)
{
    gp_Ax2 axis(center, gp_Dir(0, 0, 1));
    BRepPrimAPI_MakeCylinder cylinder(axis, radius, height);
    return cylinder.Shape();
}

const TopoDS_Shape make_hollow_cylinder(double radius, double height, double thickness, gp_Pnt center)
{
    gp_Ax2 axis(center, gp_Dir(0, 0, 1));
    BRepPrimAPI_MakeCylinder cylinder(axis, radius, height);
    BRepPrimAPI_MakeCylinder hollow(axis, radius - thickness, height);
    BRepAlgoAPI_Cut cut(cylinder.Shape(), hollow.Shape());
    return cut.Shape();
}

void gen_MDF()
{
    auto& library = Material_Library::get_library();

    // User defined material
    Explosive RDX(1.67e-3, 7420);
    RDX.eos = new Jones_Wilkins_Less(RDX.id,
        611.3e5,
        10.65e5,
        4.4,
        1.2,
        0.32,
        6530);
    RDX.name = "my_RDX";

    // Material
    //  Use Method 1, get from library
    Model Separator(1, "Separator", "./Model/MDF/101FENLIBAN.STEP", library.get["2A14T6"]);
    Model Cover(2, "Cover", "./Model/MDF/201BAOHUZHAO.STEP", library.get["2A14T4"]);
    Model Connector(3, "Connector", "./Model/MDF/301JIEBAN.STEP", library.get["2A14T6"]);
    Model Board(4, "Board", "./Model/MDF/401CEBAN_FIX.STEP", library.get["2A14T6"]);
    //  Use Method 2, direct
    Model Bolts(5, "Bolts", "./Model/MDF/Bolts.STEP", &Material_Library::steel);
    //  Or user defined material
    // Model Rdx(6, "RDX", "./Model/MDF/RDX_20.STEP", &RDX);
    // Model PbRing(7, "PbRing", "./Model/MDF/PbRing_20_120.STEP", library.get["Pb"]);
    Model Rdx(6, "RDX");
    Model PbRing(7, "PbRing");
    const double rdx_radius = 0.3;
    const double pb_ring_thicknes = 1;
    const gp_Pnt center(1.8, 0, 360);
    Rdx.shape = make_cylinder(rdx_radius, 40, center);
    PbRing.shape = make_hollow_cylinder(rdx_radius + pb_ring_thicknes, 40, pb_ring_thicknes, center);
    Rdx.material = &RDX;
    PbRing.material = library.get["Pb"];

    // Fill with particles
#ifdef NDEBUG
    const bool verbose = true;
    Separator.fill_with_particle(1, verbose);
    Cover.fill_with_particle(1, verbose);
    Connector.fill_with_particle(1.5, verbose);
    Board.fill_with_particle(1.5, verbose);
    Bolts.fill_with_particle(1, verbose);
    Rdx.fill_with_particle(0.5, verbose);
    PbRing.fill_with_particle(0.5, verbose);
#else
    Separator.fill_with_particle_sequential(2);
    Cover.fill_with_particle_sequential(2);
    Connector.fill_with_particle_sequential(3);
    Board.fill_with_particle_sequential(3);
    Bolts.fill_with_particle_sequential(2);
    Rdx.fill_with_particle_sequential(1);
    PbRing.fill_with_particle_sequential(1);
#endif

    // Another kind of RDX particle distribution
    /*
        double x = 1.8, y = 0, z = 400;
        double radius = 0.48;
        Rdx.dx = 0.25;
        size_t num_r = std::lround(radius / Rdx.dx + 0.5);
        size_t num_theta = 1;
        double theta = 2 * M_PI / (double)(num_theta), phase = M_PI;
        size_t num_z = std::lround(40 / Rdx.dx + 0.5);
        for (size_t k = 0; k < num_z; k++) {
            for (size_t i = 0; i < num_r; i++) {
                for (size_t j = 0; j < num_theta; j++) {
                    double _r = radius * (i + 0.5) / num_r;
                    double x_ = x + _r * cos(theta * j + phase);
                    double y_ = y + _r * sin(theta * j + phase);
                    double z_ = z - (k + 0.5) * Rdx.dx;
                    Rdx.particles.emplace_back(x_, y_, z_);
                }
            }
        }
     */

    auto model_list = { &Separator, &Cover, &Connector, &Board, &Bolts, &Rdx, &PbRing };

    // for (auto& model : model_list) {
    //     std::cout << model->name << ": " << model->particles.size() << std::endl;
    // }

    // Duplicate particles along z axis 4 times
    for (auto&& model : model_list) {
        auto& P = model->particles;
        int duplicate_num = 4;
        size_t original_size = P.size();
        P.reserve(original_size * 5);
        for (size_t d = 1; d <= duplicate_num; ++d) {
            for (size_t i = 0; i < original_size; ++i) {
                P.emplace_back(P[i].X(), P[i].Y(), P[i].Z() - 40 * d);
            }
        }
    }

    MPM_File MDF("MDF");
    MDF.unit = MPM_File::UNIT::MMGS_ms;
    for (auto&& model : model_list) {
        MDF.add(*model);
    }

    MDF.dx = 1;
    MDF.dCell_scale = 2;
    MDF.end_time = 0.1;
    MDF.out_time = 5e-4;
    MDF.rpt_time = 1.25e-4;
    MDF.dt_scale = 0.1;

    MDF.down_extend.x = 20;
    MDF.up_extend.x = 20;
    MDF.down_extend.z = 40 * 4 + 0.5 * MDF.dx;
    MDF.up_extend.z = 0.5 * MDF.dx;

    typedef MPM_File::BOUNDARY bound;
    MDF.down_boundary.z = (int)bound::SYMMETRY;
    MDF.down_boundary.y = (int)bound::FIX;
    MDF.up_boundary.y = (int)bound::SYMMETRY;

    MDF.detonation_point.x = 1.8;
    MDF.detonation_point.z = 400;
    MDF.Jaumann = true;
    MDF.Contact = true;
    MDF.contact_parameters.type = MPM_File::CONTACT::LAGRANGIAN;

    MDF.write();
    // delete RDX.eos;
}

void gen_PZG()
{
    auto& library = Material_Library::get_library();

    Model Separator(1, "Separator", "./Model/PZG/101FENLIBAN.STEP", library.get["2A14T6"]);
    Model Cover(2, "Cover", "./Model/PZG/201BAOHUZHAO.STEP", library.get["2A14T4"]);
    Model Connector(3, "Connector", "./Model/PZG/301CEBAN.STEP", library.get["2A14T6"]);
    Model Board(4, "Board", "./Model/PZG/901HUXINGBAN.STEP", library.get["2A14T6"]);
    Model Bolts(5, "Bolts", "./Model/PZG/Bolts.STEP", library.get["steel"]);
    Model Rdx(6, "RDX", "./Model/PZG/RDX_67.step", library.get["RDX"]);

    const bool verbose = true;
    Separator.fill_with_particle(1, verbose);
    Cover.fill_with_particle(1, verbose);
    Connector.fill_with_particle(1.5, verbose);
    Board.fill_with_particle(1.5, verbose);
    Bolts.fill_with_particle(1, verbose);
    Rdx.fill_with_particle(0.5, verbose);

    auto model_list = { &Separator, &Cover, &Connector, &Board, &Bolts, &Rdx };

    for (auto&& model : model_list) {
        auto& P = model->particles;
        int duplicate_num = 4;
        size_t original_size = P.size();
        P.reserve(original_size * 5);
        for (size_t d = 1; d <= duplicate_num; ++d) {
            for (size_t i = 0; i < original_size; ++i) {
                P.emplace_back(P[i].X(), P[i].Y(), P[i].Z() - 40 * d);
            }
        }
    }

    MPM_File PZG("PZG");
    for (auto&& model : model_list) {
        PZG.add(*model);
    }

    PZG.dx = 0.75;
    PZG.dCell_scale = 2;
    PZG.end_time = 0.2;
    PZG.out_time = 5e-4;
    PZG.rpt_time = 1.25e-4;

    PZG.down_extend.x = 15;
    PZG.up_extend.x = 15;
    PZG.down_extend.z = 40 * 4 + 0.5 * PZG.dx;
    PZG.up_extend.z = 0.5 * PZG.dx;

    PZG.down_boundary.z = (int)MPM_File::BOUNDARY::SYMMETRY;
    PZG.down_boundary.y = (int)MPM_File::BOUNDARY::FIX;
    PZG.up_boundary.y = (int)MPM_File::BOUNDARY::SYMMETRY;

    PZG.detonation_point.x = 3.6;
    PZG.detonation_point.y = -0.5;
    PZG.detonation_point.z = 400;
    PZG.Jaumann = true;
    PZG.Contact = true;
    PZG.contact_parameters.type = MPM_File::CONTACT::LAGRANGIAN;

    PZG.write();
}

int main()
{
    std::cout << "***MDF***" << std::endl;
    gen_MDF();
    std::cout << "Done" << std::endl;

    std::cout << "\n\n";

    std::cout << "***PZG***" << std::endl;
    gen_PZG();
    std::cout << "Done" << std::endl;
}
