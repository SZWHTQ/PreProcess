// Usage Example

#include <iostream>
#include <math.h>
#include <string>
#include <vector>

#include "gp_Pnt.hxx"

#include "Explosive.h"
#include "JonesWilkinsLee.h"
#include "MPMfile.h"
#include "MaterialLibrary.h"
#include "Model.h"

int main()
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
    RDX.name = "RDX";

    // Material
    //  Use Method 1, get from library
    Model Separator(1, "Separator", "./Model/101FENLIBAN.STEP", library.get["2A14T6"]);
    Model Cover(2, "Cover", "./Model/201BAOHUZHAO.STEP", library.get["2A14T4"]);
    Model Connector(3, "Connector", "./Model/301JIEBAN.STEP", library.get["2A14T6"]);
    Model Board(4, "Board", "./Model/401CEBAN_FIX.STEP", library.get["2A14T6"]);
    //  Use Method 2, direct
    Model Bolts(5, "Bolts", "./Model/Bolts.STEP", &Material_Library::steel);
    //  Or user defined material
    Model Rdx(6, "RDX", "./Model/RDX_48_COMSOL.STEP", &RDX);

    // Fill with particles
// #ifdef NDEBUG
    Separator.fill_with_particle(0.3);
    Cover.fill_with_particle(0.3);
    Connector.fill_with_particle(1);
    Board.fill_with_particle(1);
    Bolts.fill_with_particle(0.5);
    Rdx.fill_with_particle(0.12);
// #else
//     Separator.fill_with_particle(3);
//     Cover.fill_with_particle(3);
//     Connector.fill_with_particle(5);
//     Board.fill_with_particle(5);
//     Bolts.fill_with_particle(2);
//     Rdx.fill_with_particle(0.12);
// #endif
    // double x = 1.8, y = 0, z = 400;
    // double radius = 0.48;
    // Rdx.dx = 0.1;
    // size_t num_r = (size_t)(radius / Rdx.dx + 0.5);
    // size_t num_theta = 8;
    // double theta = 2 * M_PI / num_theta;
    // size_t num_z = (size_t)(40 / Rdx.dx + 0.5);
    // for (size_t k = 0; k < num_z; k++) {
    //     for (size_t i = 0; i < num_r; i++) {
    //         for (size_t j = 0; j < num_theta; j++) {
    //             double _r = radius * (i + 0.5) / num_r;
    //             double x_ = x + _r * cos(theta * j);
    //             double y_ = y + _r * sin(theta * j);
    //             double z_ = z - (k + 0.5) * Rdx.dx;
    //             Rdx.particles.push_back(gp_Pnt(x_, y_, z_));
    //         }
    //     }
    // }

    MPM_File MDF("MDF");
    MDF.unit = MPM_File::UNIT::MMGS_ms;
    MDF.add(Separator);
    MDF.add(Cover);
    MDF.add(Connector);
    MDF.add(Board);
    MDF.add(Bolts);
    MDF.add(Rdx);

    MDF.dx = 0.3;
    MDF.dCell_scale = 1.8;
    MDF.end_time = 0.05;
    MDF.out_time = 5e-4;
    MDF.rpt_time = 1.25e-4;

    MDF.down_extend.x = 15;
    MDF.up_extend.x = 15;

    MDF.up_boundary.y = (int)MPM_File::BOUNDARY::SYMMETRY;

    MDF.detonation_point.x = 1.8;
    MDF.detonation_point.z = 400;
    MDF.Jaumann = true;
    MDF.Contact = true;
    MDF.contact_parameters.type = MPM_File::CONTACT::LAGRANGIAN;

    MDF.write();
}