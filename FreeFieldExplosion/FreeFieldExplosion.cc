#include <cmath>
#include <memory>

#include "BRepAlgoAPI_Cut.hxx"
#include "BRepPrimAPI_MakeBox.hxx"
#include "BRepPrimAPI_MakeCylinder.hxx"
#include "Explosive.h"
#include "JonesWilkinsLee.h"
#include "MaterialLibrary.h"
#include "Model.h"
#include "MpmFile.h"

int main() {
    auto& library = MaterialLibrary::get_library();
    Model Air(1, "Air");
    Model TNT(2, "TNT");

    const double thickness = 0.1;

    const double tntDensity = 1630;
    const double tntDetonationVelocity = 6930;
    const double tntMass = 5;
    const double tntRadius = std::sqrt(tntMass / tntDensity / 1 / M_PI);
    const gp_Pnt center(0, 1.5, 0);
    const gp_Ax2 axis(center, gp_Dir(0, 0, 1));
    TNT.shape = std::make_shared<TopoDS_Shape>(
        BRepPrimAPI_MakeCylinder(axis, tntRadius, thickness).Shape());
    TNT.material = new Explosive(tntDensity, tntDetonationVelocity);
    TNT.material->eos = new JonesWilkinsLee(TNT.material->id, 3.712e11, 3.231e9,
                                            4.15, 0.95, 0.3, 7e9);

    const double airLength = 20;
    const double airHeight = 7.5;
    const gp_Pnt leftBottomCorner(-airLength / 2, 0, 0);
    auto& base =
        BRepPrimAPI_MakeBox(leftBottomCorner, airLength, airHeight, thickness)
            .Shape();
    Air.shape = std::make_shared<TopoDS_Shape>(
        BRepAlgoAPI_Cut(base, *TNT.shape).Shape());
    Air.material = library.get["2A14T6"];

    Air.fill(0.5);
    TNT.fill(0.5);

    MpmFile ffe("Free Field Explosion");
    auto model_list = {&Air, &TNT};
    for (auto model : model_list) {
        ffe.add(*model);
    }

    ffe.dx = 5e-2;
    ffe.dCell_scale = 1e-1;
    ffe.end_time = 1e-2;
    ffe.out_time = 2e-4;
    ffe.rpt_time = 5e-5;

    ffe.down_boundary.x = (int)MpmFile::BOUNDARY::SYMMETRY;
    ffe.down_boundary.y = (int)MpmFile::BOUNDARY::FIX;
    ffe.down_boundary.z = (int)MpmFile::BOUNDARY::SYMMETRY;
    ffe.up_boundary.x = (int)MpmFile::BOUNDARY::SYMMETRY;
    ffe.up_boundary.y = (int)MpmFile::BOUNDARY::SYMMETRY;
    ffe.up_boundary.z = (int)MpmFile::BOUNDARY::SYMMETRY;

    ffe.detonation_point.x = 0;
    ffe.detonation_point.y = 1.5;
    ffe.detonation_point.z = thickness / 2;

    ffe.write();
}