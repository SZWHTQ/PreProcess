#include <iostream>
#include <cmath>
#include <unordered_set>

#include "gp_Pnt.hxx"

#include "MPMfile.h"
#include "Material.h"
#include "Model.h"

size_t MPM_File::get_component_num()
{
    std::unordered_set<size_t> components;
    for (const auto& model : models) {
        components
            .insert(model->id);
    }
    component_num = components.size();
    return components.size();
}

size_t MPM_File::get_particle_num()
{
    size_t num = 0;
    for (const auto& model : models) {
        num += model
                   ->particles.size();
    }
    particle_num = num;
    return particle_num;
}

size_t MPM_File::get_material_num()
{
    std::unordered_set<size_t> material_id;
    for (const auto& model : models) {
        material_id
            .insert(model
                        ->material
                        ->id);
    }
    component_num = material_id.size();
    return component_num;
}

std::tuple<gp_Pnt, gp_Pnt> MPM_File::get_max_min_coor()
{
    gp_Pnt max_coor, min_coor;
    max_coor.SetX(std::numeric_limits<double>::min());
    max_coor.SetY(std::numeric_limits<double>::min());
    max_coor.SetZ(std::numeric_limits<double>::min());
    min_coor.SetX(std::numeric_limits<double>::max());
    min_coor.SetY(std::numeric_limits<double>::max());
    min_coor.SetZ(std::numeric_limits<double>::max());

    for (const auto& model : models) {
        auto [max, min] = model->get_max_min_coor();
        if (max.X() > max_coor.X()) {
            max_coor.SetX(max.X());
        }
        if (max.Y() > max_coor.Y()) {
            max_coor.SetY(max.Y());
        }
        if (max.Z() > max_coor.Z()) {
            max_coor.SetZ(max.Z());
        }
        if (min.X() < min_coor.X()) {
            min_coor.SetX(min.X());
        }
        if (min.Y() < min_coor.Y()) {
            min_coor.SetY(min.Y());
        }
        if (min.Z() < min_coor.Z()) {
            min_coor.SetZ(min.Z());
        }
    }

    return std::tie(max_coor, min_coor);
}

void MPM_File::add(const Model& model)
{
    models.push_back(std::make_unique<Model>(model));
}

void MPM_File::write()
{
    std::ofstream file;
    file.open(name + ".mpm", std::ios::out | std::ios::trunc);
    file << "mpm3d *** " << name << " ***\n";
    switch (unit) {
    case UNIT::MMGS_ms:
        file << "! Unit: mm g N ms MPa\n";
        break;
    default:
        break;
    }
    file << "nbco " << get_component_num() << "\n";
    file << "nbbo " << models.size() << "\n";
    file << "nbmp " << get_particle_num() << "\n";
    file << "nmat " << get_material_num() << "\n";

    // Grid Span & Cell Size
    file << "\n";
    auto [max, min] = get_max_min_coor();
    file << "spx " << min.X() - down_extend.x << " "
         << max.X() + up_extend.x << "\n";
    file << "spy " << min.Y() - down_extend.y << " "
         << max.Y() + up_extend.y << "\n";
    file << "spz " << min.Z() - down_extend.z << " "
         << max.Z() + up_extend.z << "\n";
    file << "dcell " << dx * dCell_scale << "\n";

    // Boundary Condition
    file << "\n";
    file << "fixed "
         << down_boundary.x << " "
         << up_boundary.x << " "
         << down_boundary.y << " "
         << up_boundary.y << " "
         << down_boundary.z << " "
         << up_boundary.z << "\n";

    // Detonation Point
    file << "\n";
    file << "deto "
         << detonation_point.x << " "
         << detonation_point.y << " "
         << detonation_point.z << "\n";

    // Material
    std::unordered_set<size_t> id_set;
    size_t count = 0;
    file << "\n";
    file << "material\n";
    for (auto& model : models) {
        auto& mat = model->material;
        bool is_new = id_set
                          .insert(mat->id)
                          .second;
        if (is_new) {
            ++count;
            mat->local_index = count;
            file << mat->to_string()
                 << "\n";
        }
    }
    //  EoS
    id_set.clear();
    for (auto& model : models) {
        auto& eos = model->material->eos;
        bool is_new = id_set
                          .insert(eos->id)
                          .second;
        if (is_new) {
            file << eos->to_string(model
                                       ->material
                                       ->local_index)
                 << "\n";
        }
    }

    // PostProcess Parameter
    file << "\n";
    if (post_process.paraview) {
        file << "paraview\n";
    }
    if (post_process.tecplot) {
        file << "tecplot\n";
    }
    if (post_process.unv) {
        file << "unvx\n";
    }

    // Time Step
    file << "\n";
    file << "dtscale " << dt_scale << "\n";
    file << "endt " << end_time << "\n";
    file << "outtime " << out_time << "\n";
    file << "rpttime " << rpt_time << "\n";

    // Algorithm
    file << "\n";
    if (GIMP) {
        file << "gimp\n";
    }
    switch (algorithm) {
    case ALGORITHM::MUSL:
        file << "musl on\n";
        break;
    case ALGORITHM::USF:
        file << "usf on\n";
        break;
    case ALGORITHM::USL:
        file << "usl on\n";
        break;
    default:
        break;
    }
    if (Jaumann) {
        file << "jaum on\n";
    }
    if (Contact) {
        file << "contact\n";
        switch (contact_parameters.type) {
        case CONTACT::LAGRANGIAN:
            file << "lagr "
                 << contact_parameters.friction << " "
                 << contact_parameters.body_norm << "\n";
            break;
        case CONTACT::PENALTY:
            std::clog << "Unfinished\n";
        default:
            break;
        }
    }

    // Particle Data
    count = 0;
    for (auto& model : models) {
        file << "\n";
        file << "Particle "
             << "point " << model->id << " " << model->particles.size() << "\n";
        for (auto& particle : model->particles) {
            file << ++count << " "
                 << model
                        ->material
                        ->local_index
                 << " "
                 << model
                        ->material
                        ->density
                    * pow(model->dx, 3)
                 << " "
                 << particle.X() << " "
                 << particle.Y() << " "
                 << particle.Z() << "\n";
        }
    }
    file << "\nendi\n";
    file.close();
}
