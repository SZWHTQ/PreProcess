#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
// #include <mutex>
#include <omp.h>
#include <string>
// #include <thread>
#include <tuple>

#include "BRepBuilderAPI_MakeVertex.hxx"
#include "BRepClass3d_SolidClassifier.hxx"
#include "BRep_Tool.hxx"
#include "STEPControl_Reader.hxx"
#include "StlAPI_Reader.hxx"
#include "TopAbs.hxx"
#include "TopExp.hxx"

#include "ANSI.h"
#include "Material.h"
#include "Model.h"
#include "ThreadPool.h"
#include "Timer.h"

bool compare_file_extension(const std::string& filename, const std::string& extension)
{
#include <algorithm> // Add this line

    size_t dotPosition = filename.find_last_of(".");

    if (dotPosition != std::string::npos) {
        std::string fileExtension = filename.substr(dotPosition + 1);

        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

        std::string targetExtension = extension;
        std::transform(targetExtension.begin(), targetExtension.end(), targetExtension.begin(), ::tolower);

        return (fileExtension == targetExtension);
    }

    return false;
}

Model::Model(size_t _id, std::string _model_name, std::string _file_path, Material* _material)
{
    id = _id;
    name = _model_name;
    material = _material;
    if (compare_file_extension(_file_path, "stp") || compare_file_extension(_file_path, "step")) {
        STEPControl_Reader stepReader;
        int status = stepReader.ReadFile(_file_path.c_str());
        stepReader.TransferRoots();

        if (status != IFSelect_RetDone) {
            std::cerr << "Error reading STEP file." << std::endl;
            return;
        }
        // std::clog << "ReadFile status: " << status << std::endl;

        // Check if there are objects in the document
        if (stepReader.NbShapes() == 0) {
            std::cout << "No shapes found in the STEP file." << std::endl;
            return;
        }

        // Transfer the contents of the STEP file to a TopoDS_Shape
        shape = stepReader.Shape();
    } else if (compare_file_extension(_file_path, "stl")) {
        StlAPI_Reader stlReader;
        bool status = stlReader.Read(shape, _file_path.c_str());

        if (!status) {
            std::cerr << "Error reading STL file." << std::endl;
            return;
        }
    }
}

std::tuple<gp_Pnt, gp_Pnt> Model::get_max_min_coor()
{
    gp_Pnt max_coor, min_coor;
    max_coor.SetX(-std::numeric_limits<double>::max());
    max_coor.SetY(-std::numeric_limits<double>::max());
    max_coor.SetZ(-std::numeric_limits<double>::max());
    min_coor.SetX(std::numeric_limits<double>::max());
    min_coor.SetY(std::numeric_limits<double>::max());
    min_coor.SetZ(std::numeric_limits<double>::max());

    for (TopExp_Explorer explorer(shape, TopAbs_VERTEX); explorer.More(); explorer.Next()) {
        TopoDS_Vertex vertex = TopoDS::Vertex(explorer.Current());
        gp_Pnt point = BRep_Tool::Pnt(vertex);

        // Update minimum and maximum coordinates
        min_coor.SetX(std::min(min_coor.X(), point.X()));
        min_coor.SetY(std::min(min_coor.Y(), point.Y()));
        min_coor.SetZ(std::min(min_coor.Z(), point.Z()));

        max_coor.SetX(std::max(max_coor.X(), point.X()));
        max_coor.SetY(std::max(max_coor.Y(), point.Y()));
        max_coor.SetZ(std::max(max_coor.Z(), point.Z()));
    }

    return std::tie(max_coor, min_coor);
}

bool Model::contain(gp_Pnt* point)
{
    // Create a vertex from the test point
    // TopoDS_Vertex vertex = BRepBuilderAPI_MakeVertex(testPoint);

    // Use BRepClass3d_SolidClassifier to check if the vertex is inside the shape
    BRepClass3d_SolidClassifier classifier(shape);
    classifier.Perform(*point, 1e-9);

    // Check the result of the classification
    return (classifier.State() == TopAbs_IN);
}

void Model::fill_with_particle(double _dx, bool verbose)
{
    Model::dx = _dx;
    // Get the max and min coordinate of the model
    auto [max_coor, min_coor] = get_max_min_coor();

    // Calculate the number of particles in each direction
    size_t x_num = (size_t)((max_coor.X() - min_coor.X()) / dx + 1.5);
    size_t y_num = (size_t)((max_coor.Y() - min_coor.Y()) / dx + 1.5);
    size_t z_num = (size_t)((max_coor.Z() - min_coor.Z()) / dx + 1.5);

    if (verbose) {
        std::cout << "Filling " << name << "..." << std::endl;
        std::cout << "Max coordinate: " << max_coor.X() << ", " << max_coor.Y() << ", " << max_coor.Z() << std::endl;
        std::cout << "Min coordinate: " << min_coor.X() << ", " << min_coor.Y() << ", " << min_coor.Z() << std::endl;
        std::cout << "Total number: " << x_num * y_num * z_num << std::endl;
        std::cout << "Number in each direction: " << x_num << ", " << y_num << ", " << z_num << std::endl;
        std::cout << std::fixed;
    }

    // Fill the model with particles
    size_t total_num = x_num * y_num * z_num;
    size_t iter = 0, interval = 1e2;
    double percent = 0, elapsed = 0, iter_per_second = 0;
    Timer T, t;
    particles.reserve(total_num);
    for (size_t i = 0; i < x_num; i++) {
        for (size_t j = 0; j < y_num; j++) {
            for (size_t k = 0; k < z_num; k++) {
                gp_Pnt point(
                    min_coor.X() + (i + 0.5) * dx,
                    min_coor.Y() + (j + 0.5) * dx,
                    min_coor.Z() + (k + 0.5) * dx);
                if (contain(&point)) {
                    particles.emplace_back(point.X(), point.Y(), point.Z());
                }
                if (verbose) {
                    ++iter;
                    percent = (double)iter / total_num;
                    elapsed = T.elapsed();
                    if (iter % interval == 0) {
                        iter_per_second = interval / (t.elapsed() + std::numeric_limits<double>::min());
                        t.reset();
                    }
                    std::cout << "\r"
                              << std::setprecision(2)
                              << "Progress: " << percent * 100 << "%, "
                              << std::setprecision(1)
                              << "Elapsed: " << elapsed << "s, "
                              << "Estimated: " << elapsed / (percent + std::numeric_limits<double>::min()) * (1 - percent) << "s, "
                              << iter_per_second << "it/s, "
                              << "Particle number: " << particles.size();
                    std::cout.flush();
                }
            }
        }
    }

    if (verbose) {
        std::cout.flush();
        std::cout << ANSI_GREEN;
        std::cout << "\nTotal elapsed: " << T.elapsed() << "s, "
                  << "Particle number: " << particles.size()
                  << std::endl;
        std::cout << std::setprecision(-1);
        std::cout << ANSI_RESET << std::endl;
    }
}

void Model::fill_with_particle_omp(double _dx, bool verbose)
{
    Model::dx = _dx;
    // Get the max and min coordinate of the model
    auto [max_coor, min_coor] = get_max_min_coor();

    // Calculate the number of particles in each direction
    size_t x_num = (size_t)((max_coor.X() - min_coor.X()) / dx + 1.5);
    size_t y_num = (size_t)((max_coor.Y() - min_coor.Y()) / dx + 1.5);
    size_t z_num = (size_t)((max_coor.Z() - min_coor.Z()) / dx + 1.5);

    if (verbose) {
        {
            {
                std::cout << "Filling " << name << "..." << std::endl;
                std::cout << "Max coordinate: " << max_coor.X() << ", " << max_coor.Y() << ", " << max_coor.Z() << std::endl;
                std::cout << "Min coordinate: " << min_coor.X() << ", " << min_coor.Y() << ", " << min_coor.Z() << std::endl;
                std::cout << "Total number: " << x_num * y_num * z_num << std::endl;
                std::cout << "Number in each direction: " << x_num << ", " << y_num << ", " << z_num << std::endl;
                std::cout << std::fixed;
            }
        }
    }

    // Fill the model with particles
    size_t total_num = x_num * y_num * z_num;
    size_t iter = 0, interval = 1e2;
    double percent = 0, elapsed = 0, iter_per_second = 0;
    Timer T, t;
    const double min_x = min_coor.X(), min_y = min_coor.Y(), min_z = min_coor.Z();
    particles.reserve(total_num);
#pragma omp parallel
    {
#pragma omp for collapse(3)
        for (size_t i = 0; i < x_num; i++) {
            for (size_t j = 0; j < y_num; j++) {
                for (size_t k = 0; k < z_num; k++) {
                    gp_Pnt point(
                        min_x + (i + 0.5) * dx,
                        min_y + (j + 0.5) * dx,
                        min_z + (k + 0.5) * dx);

                    if (contain(&point)) {
#pragma omp critical
                        {
                            particles.emplace_back(point.X(), point.Y(), point.Z());
                        }
                    }

                    if (verbose && omp_get_thread_num() == 0) {
                        {
                            ++iter;
                            percent = (double)iter / total_num;
                            elapsed = T.elapsed();
                            if (iter % interval == 0) {
                                iter_per_second = interval / (t.elapsed() + std::numeric_limits<double>::min());
                                t.reset();
                            }
                            std::cout << "\r"
                                      << std::setprecision(2)
                                      << "Progress: " << percent * 100 << "%, "
                                      << std::setprecision(1)
                                      << "Elapsed: " << elapsed << "s, "
                                      << "Estimated: " << elapsed / (percent + std::numeric_limits<double>::min()) * (1 - percent) << "s, "
                                      << iter_per_second << "it/s, "
                                      << "Particle number: " << particles.size();
                            std::cout.flush();
                        }
                    }
                }
            }
        }
    }

    if (verbose) {
        std::cout.flush();
        std::cout << ANSI_GREEN;
        std::cout << "\nTotal elapsed: " << T.elapsed() << "s, "
                  << "Particle number: " << particles.size()
                  << std::endl;
        std::cout << std::setprecision(-1);
        std::cout << ANSI_RESET << std::endl;
    }
}

void Model::fill_with_particle_parallel(double _dx, bool verbose)
{
    std::cout << "Fatal error, please use `fill_with_particle`\n";
    return;
    Model::dx = _dx;
    auto [max_coor, min_coor] = get_max_min_coor();

    size_t x_num = (size_t)((max_coor.X() - min_coor.X()) / dx + 1.5);
    size_t y_num = (size_t)((max_coor.Y() - min_coor.Y()) / dx + 1.5);
    size_t z_num = (size_t)((max_coor.Z() - min_coor.Z()) / dx + 1.5);

    std::mutex particles_mutex; // Mutex for synchronizing access to particles vector

    if (verbose) {
        std::cout << "Filling " << name << "..." << std::endl;
        std::cout << "Max coordinate: " << max_coor.X() << ", " << max_coor.Y() << ", " << max_coor.Z() << std::endl;
        std::cout << "Min coordinate: " << min_coor.X() << ", " << min_coor.Y() << ", " << min_coor.Z() << std::endl;
        std::cout << "Total number: " << x_num * y_num * z_num << std::endl;
        std::cout << "Number in each direction: " << x_num << ", " << y_num << ", " << z_num << std::endl;
        std::cout << std::fixed;
    }

    size_t total_num = x_num * y_num * z_num;
    size_t count = 0, interval = 1e2;
    double percent = 0, elapsed = 0, iter_per_second = 0;
    Timer T, t;

    ThreadPool thread_pool(std::thread::hardware_concurrency());
    const double min_x = min_coor.X(), min_y = min_coor.Y(), min_z = min_coor.Z();
    particles.reserve(total_num);

    auto thread_function = [&](size_t i, size_t j, size_t k) {
        gp_Pnt point(
            min_x + (i + 0.5) * dx,
            min_y + (j + 0.5) * dx,
            min_z + (k + 0.5) * dx);

        if (contain(&point)) {
            std::lock_guard<std::mutex> lock(particles_mutex);
            particles.emplace_back(point.X(), point.Y(), point.Z());
        }

        if (verbose) {
            std::lock_guard<std::mutex> lock(particles_mutex);
            count++;
            percent = (double)count / total_num;
            elapsed = T.elapsed();
            if (count % interval == 0) {
                iter_per_second = interval / (t.elapsed() + std::numeric_limits<double>::min());
                t.reset();
            }
            std::cout << "\r"
                      << std::setprecision(2)
                      << "Progress: " << percent * 100 << "%, "
                      << std::setprecision(1)
                      << "Elapsed: " << elapsed << "s, "
                      << "Estimated: " << elapsed / (percent + std::numeric_limits<double>::min()) * (1 - percent) << "s, "
                      << iter_per_second << "it/s, "
                      << "Particle number: " << particles.size();
            std::cout.flush();
        }
    };

    for (size_t i = 0; i < x_num; i++) {
        for (size_t j = 0; j < y_num; j++) {
            for (size_t k = 0; k < z_num; k++) {
                /* Fatal error */
                // thread_pool.enqueue(std::bind(thread_function, i, j, k));
            }
        }
    }

    // Wait for all tasks to complete
    // Note: This assumes the tasks are not dependent on each other
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (verbose) {
        std::cout.flush();
        std::cout << ANSI_GREEN;
        std::cout << "\nTotal elapsed: " << T.elapsed() << "s, "
                  << "Particle number: " << particles.size()
                  << std::endl;
        std::cout << std::setprecision(-1);
        std::cout << ANSI_RESET << std::endl;
    }
}