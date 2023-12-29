#include <iostream>
#include <string>
#include <vector>

#include "MPMfile.h"
#include "Model.h"
// #include "Material.h"
#include "Explosive.h"
#include "JohnsonCook.h"
// #include "EquationOfState.h"
#include "JonesWilkinsLee.h"
#include "MieGruneisen.h"

int main()
{
    Johnson_Cook _2A14T6(
        2.8e-3,
        6.89e4,
        0.33,
        402,
        635,
        0.9,
        0.0075,
        1.426,
        293,
        863,
        8.8e2,
        1e-6,
        0.116,
        0.211,
        -2.172,
        0.012,
        -0.01256,
        1);
    _2A14T6.eos = new Mie_Gruneisen(
        _2A14T6.id,
        4080,
        1.86,
        0,
        0,
        2.7720001);

    Johnson_Cook _2A14T4(
        2.8e-3,
        6.89e4,
        0.33,
        295,
        635,
        0.9,
        0.0075,
        1.429,
        293,
        863,
        8.8e2,
        1e-6,
        0.116,
        0.211,
        -2.172,
        0.012,
        -0.01256,
        1.1);
    _2A14T4.eos = new Mie_Gruneisen(
        _2A14T4.id,
        4080,
        1.86,
        0,
        0,
        2.7720001);

    Johnson_Cook steel(
        7.85e-3,
        2e5,
        0.28,
        525,
        101,
        0.081,
        0.1739,
        1.635,
        293,
        1800,
        47.7,
        1e-6,
        0.0705,
        1.732,
        -0.54,
        -0.0123,
        0,
        1);
    steel.eos = new Mie_Gruneisen(
        steel.id,
        4569,
        1.49,
        0,
        0,
        2.1700001,
        0.46);

    Explosive RDX(1.67e-3, 7420);
    RDX.eos = new Jones_Wilkins_Less(
        RDX.id,
        611.3e5,
        10.65e5,
        4.4,
        1.2,
        0.32,
        6530);

    Model Separator(1, "Separator", "./Model/101FENLIBAN.STEP", &_2A14T6);
    Model Cover(2, "Cover", "./Model/201BAOHUZHAO.STEP", &_2A14T4);
    Model Connector(3, "Connector","./Model/301JIEBAN.STEP", &_2A14T6);
    Model Board(4, "Board", "./Model/401CEBAN_FIX.STEP", &_2A14T6);
    Model Bolts(5, "Bolts", "./Model/Bolts.STEP", &steel);
    Model Rdx(6, "RDX", "./Model/RDX_48.STEP", &RDX);

    Separator.fill_with_particle(0.3);
    Cover.fill_with_particle(0.3);
    Connector.fill_with_particle(1);
    Board.fill_with_particle(1);
    Bolts.fill_with_particle(1);
    Rdx.fill_with_particle(0.12);

    MPM_File mpm_file("MDF");
    mpm_file.models.push_back(std::make_unique<Model>(Separator));
    mpm_file.models.push_back(std::make_unique<Model>(Cover));
    mpm_file.models.push_back(std::make_unique<Model>(Connector));
    mpm_file.models.push_back(std::make_unique<Model>(Board));
    mpm_file.models.push_back(std::make_unique<Model>(Bolts));
    mpm_file.models.push_back(std::make_unique<Model>(Rdx));

    mpm_file.dx = 0.2;
    mpm_file.dCell_scale = 1.8;
    mpm_file.end_time = 0.05;
    mpm_file.out_time = 5e-4;
    mpm_file.rpt_time = 1.25e-4;

    mpm_file.down_extend.x = 15;
    mpm_file.up_extend.x = 15;

    mpm_file.up_boundary.y = (int) MPM_File::BOUNDARY::SYMMETRY;

    mpm_file.detonation_point.x = 400;
    mpm_file.detonation_point.y = 1.8;
    mpm_file.Jaumann = true;
    mpm_file.Contact = true;
    mpm_file.contact_parameters.type = MPM_File::CONTACT::LAGRANGIAN;

    mpm_file.write();

}