// Usage Example

#include <iostream>
#include <string>
#include <vector>

#include "Explosive.h"
#include "MPMfile.h"
#include "Model.h"
// #include "JohnsonCook.h"
#include "JonesWilkinsLee.h"
// #include "MieGruneisen.h"
#include "MaterialLibrary.h"

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
    Model Rdx(6, "RDX", "./Model/RDX_48.STEP", &RDX);

#ifdef NDEBUG
    Separator.fill_with_particle(0.3);
    Cover.fill_with_particle(0.3);
    Connector.fill_with_particle(1);
    Board.fill_with_particle(1);
    Bolts.fill_with_particle(1);
    Rdx.fill_with_particle(0.12);
#else
    Separator.fill_with_particle(3);
    Cover.fill_with_particle(3);
    Connector.fill_with_particle(5);
    Board.fill_with_particle(5);
    Bolts.fill_with_particle(2);
    Rdx.fill_with_particle(0.48);
#endif

    MPM_File mpm_file("MDF");
    mpm_file.unit = MPM_File::UNIT::MMGS_ms;
    mpm_file.add(Separator);
    mpm_file.add(Cover);
    mpm_file.add(Connector);
    mpm_file.add(Board);
    mpm_file.add(Bolts);
    mpm_file.add(Rdx);

    mpm_file.dx = 0.3;
    mpm_file.dCell_scale = 1.8;
    mpm_file.end_time = 0.05;
    mpm_file.out_time = 5e-4;
    mpm_file.rpt_time = 1.25e-4;

    mpm_file.down_extend.x = 15;
    mpm_file.up_extend.x = 15;

    mpm_file.up_boundary.y = (int)MPM_File::BOUNDARY::SYMMETRY;

    mpm_file.detonation_point.x = 1.8;
    mpm_file.detonation_point.z = 400;
    mpm_file.Jaumann = true;
    mpm_file.Contact = true;
    mpm_file.contact_parameters.type = MPM_File::CONTACT::LAGRANGIAN;

    mpm_file.write();
}