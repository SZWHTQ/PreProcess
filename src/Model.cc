#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
// #include <mpi.h>
#include <omp.h>

#include <string>
#include <tuple>
#include <utility>

#include "ANSI.h"
#include "BRepBndLib.hxx"
#include "BRepClass3d_SolidClassifier.hxx"
#include "BRep_Tool.hxx"
#include "Bnd_Box.hxx"
#include "Material.h"
#include "Model.h"
#include "STEPControl_Reader.hxx"
#include "StlAPI_Reader.hxx"
#include "ThreadPool.h"
#include "Timer.h"
#include "TopExp.hxx"
#include "TopoDS.hxx"

bool compare_file_extension(const std::string& filename,
                            const std::string& extension) {
    size_t dotPosition = filename.find_last_of('.');

    if (dotPosition != std::string::npos) {
        std::string fileExtension = filename.substr(dotPosition + 1);

        std::transform(fileExtension.begin(), fileExtension.end(),
                       fileExtension.begin(), ::tolower);

        std::string targetExtension = extension;
        std::transform(targetExtension.begin(), targetExtension.end(),
                       targetExtension.begin(), ::tolower);

        return (fileExtension == targetExtension);
    }

    return false;
}

Model::Model(size_t _id, std::string _model_name, const std::string& _file_path,
             Material* _material) {
    id = _id;
    name = std::move(_model_name);
    material = _material;
    if (compare_file_extension(_file_path, "stp") ||
        compare_file_extension(_file_path, "step")) {
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

std::tuple<gp_Pnt, gp_Pnt> Model::get_max_min_coor() const {
    Bnd_Box bounding_box;
    double xmin, ymin, zmin, xmax, ymax, zmax;

    BRepBndLib::Add(shape, bounding_box);
    bounding_box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

    return {gp_Pnt(xmax, ymax, zmax), gp_Pnt(xmin, ymin, zmin)};
}

bool Model::contain(const gp_Pnt& point) const {
    // Create a vertex from the test point
    // TopoDS_Vertex vertex = BRepBuilderAPI_MakeVertex(testPoint);

    // Use BRepClass3d_SolidClassifier to check if the point is inside the shape
    BRepClass3d_SolidClassifier classifier(shape);
    classifier.Perform(point, 1e-9);

    // Check the result of the classification
    return (classifier.State() == TopAbs_IN);
}

void Model::fill_with_particle(const double _dx, const bool verbose) {
    Model::dx = _dx;
    auto [max_coor, min_coor] = get_max_min_coor();

    auto x_num = (size_t)((max_coor.X() - min_coor.X()) / dx + 1.5);
    auto y_num = (size_t)((max_coor.Y() - min_coor.Y()) / dx + 1.5);
    auto z_num = (size_t)((max_coor.Z() - min_coor.Z()) / dx + 1.5);

    std::mutex
        particles_mutex;  // Mutex for synchronizing access to particles vector

    if (verbose) {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
        std::cout.tie(nullptr);
        std::cout << "Filling " << name << "..." << std::endl;
        std::cout << "Max coordinate: " << max_coor.X() << ", " << max_coor.Y()
                  << ", " << max_coor.Z() << std::endl;
        std::cout << "Min coordinate: " << min_coor.X() << ", " << min_coor.Y()
                  << ", " << min_coor.Z() << std::endl;
        std::cout << "Total number: " << x_num * y_num * z_num << std::endl;
        std::cout << "Number in each direction: " << x_num << ", " << y_num
                  << ", " << z_num << std::endl;
        std::cout << std::fixed;
    }

    Timer T;
    {
        const size_t total_num = x_num * y_num * z_num;
        const size_t interval = 5e2;
        size_t count = 0;
        const double min_x = min_coor.X(), min_y = min_coor.Y(),
                     min_z = min_coor.Z();
        double percent = 0, elapsed = 0, iter_per_second = 0;

        Timer t;

        particles.reserve(total_num);

        auto thread_function = [&](size_t i, size_t j, size_t k) {
            gp_Pnt point(min_x + (i + 0.5) * dx, min_y + (j + 0.5) * dx,
                         min_z + (k + 0.5) * dx);

            if (contain(point)) {
                std::lock_guard<std::mutex> lock(particles_mutex);
                particles.emplace_back(point);
            }

            if (verbose) {
                std::lock_guard<std::mutex> lock(particles_mutex);
                count++;
                percent = (double)count / total_num;
                elapsed = T.elapsed();
                if (count % interval == 0 || count == total_num) {
                    iter_per_second =
                        interval /
                        (t.elapsed() + std::numeric_limits<double>::min());
                    t.reset();
                    std::cout
                        << "\r" << std::setprecision(2)
                        << "Progress: " << percent * 100 << "%, "
                        << std::setprecision(1) << "Elapsed: " << elapsed
                        << "s, "
                        << "Estimated: "
                        << elapsed /
                               (percent + std::numeric_limits<double>::min()) *
                               (1 - percent)
                        << "s, " << iter_per_second << "it/s, "
                        << "Particle number: " << particles.size();
                    std::cout.flush();
                }
            }
        };

        ThreadPool pool(std::thread::hardware_concurrency());
        for (size_t i = 0; i < x_num; ++i) {
            for (size_t j = 0; j < y_num; ++j) {
                for (size_t k = 0; k < z_num; ++k) {
                    pool.enqueue([thread_function, i, j, k] {
                        return thread_function(i, j, k);
                    });
                }
            }
        }
    }

    if (verbose) {
        std::cout.flush();
        std::cout << ANSI_GREEN;
        std::cout << "\nTotal elapsed: " << T.elapsed() << "s, "
                  << "Particle number: " << particles.size() << std::endl;
        std::cout << std::setprecision(-1);
        std::cout << ANSI_RESET_COLOR << std::endl;
    }
}

void Model::fill_with_particle_omp(const double _dx, const bool verbose) {
    Model::dx = _dx;
    // Get the max and min coordinate of the model
    auto [max_coor, min_coor] = get_max_min_coor();

    // Calculate the number of particles in each direction
    auto x_num = (size_t)((max_coor.X() - min_coor.X()) / dx + 1.5);
    auto y_num = (size_t)((max_coor.Y() - min_coor.Y()) / dx + 1.5);
    auto z_num = (size_t)((max_coor.Z() - min_coor.Z()) / dx + 1.5);

    if (verbose) {
        {
            {
                std::cout << "Filling " << name << "..." << std::endl;
                std::cout << "Max coordinate: " << max_coor.X() << ", "
                          << max_coor.Y() << ", " << max_coor.Z() << std::endl;
                std::cout << "Min coordinate: " << min_coor.X() << ", "
                          << min_coor.Y() << ", " << min_coor.Z() << std::endl;
                std::cout << "Total number: " << x_num * y_num * z_num
                          << std::endl;
                std::cout << "Number in each direction: " << x_num << ", "
                          << y_num << ", " << z_num << std::endl;
                std::cout << std::fixed;
            }
        }
    }

    // Fill the model with particles
    size_t total_num = x_num * y_num * z_num;
    size_t iter = 0, interval = 1e2;
    double percent = 0, elapsed = 0, iter_per_second = 0;
    Timer T, t;
    const double min_x = min_coor.X(), min_y = min_coor.Y(),
                 min_z = min_coor.Z();
    particles.reserve(total_num);
#pragma omp parallel
    {
#pragma omp for collapse(3)
        for (long i = 0; i < x_num; i++) {
            for (long j = 0; j < y_num; j++) {
                for (long k = 0; k < z_num; k++) {
                    gp_Pnt point(min_x + (i + 0.5) * dx, min_y + (j + 0.5) * dx,
                                 min_z + (k + 0.5) * dx);

                    if (contain(point)) {
#pragma omp critical
                        {
                            particles.emplace_back(point.X(), point.Y(),
                                                   point.Z());
                        }
                    }

                    if (verbose && omp_get_thread_num() == 0) {
                        {
                            ++iter;
                            percent = (double)iter / total_num;
                            elapsed = T.elapsed();
                            if (iter % interval == 0) {
                                iter_per_second =
                                    interval /
                                    (t.elapsed() +
                                     std::numeric_limits<double>::min());
                                t.reset();
                            }
                            std::cout
                                << "\r" << std::setprecision(2)
                                << "Progress: " << percent * 100 << "%, "
                                << std::setprecision(1)
                                << "Elapsed: " << elapsed << "s, "
                                << "Estimated: "
                                << elapsed /
                                       (percent +
                                        std::numeric_limits<double>::min()) *
                                       (1 - percent)
                                << "s, " << iter_per_second << "it/s, "
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
                  << "Particle number: " << particles.size() << std::endl;
        std::cout << std::setprecision(-1);
        std::cout << ANSI_RESET_COLOR << std::endl;
    }
}

void Model::fill_with_particle_sequential(const double _dx,
                                          const bool verbose) {
    Model::dx = _dx;
    // Get the max and min coordinate of the model
    auto [max_coor, min_coor] = get_max_min_coor();

    // Calculate the number of particles in each direction
    auto x_num = (size_t)((max_coor.X() - min_coor.X()) / dx + 1.5);
    auto y_num = (size_t)((max_coor.Y() - min_coor.Y()) / dx + 1.5);
    auto z_num = (size_t)((max_coor.Z() - min_coor.Z()) / dx + 1.5);

    if (verbose) {
        std::cout << "Filling " << name << "..." << std::endl;
        std::cout << "Max coordinate: " << max_coor.X() << ", " << max_coor.Y()
                  << ", " << max_coor.Z() << std::endl;
        std::cout << "Min coordinate: " << min_coor.X() << ", " << min_coor.Y()
                  << ", " << min_coor.Z() << std::endl;
        std::cout << "Total number: " << x_num * y_num * z_num << std::endl;
        std::cout << "Number in each direction: " << x_num << ", " << y_num
                  << ", " << z_num << std::endl;
        std::cout << std::fixed;
    }

    // Fill the model with particles
    size_t total_num = x_num * y_num * z_num;
    size_t iter = 0, interval = 1e2;
    double percent, elapsed, iter_per_second = 0;
    Timer T, t;
    particles.reserve(total_num);
    for (size_t i = 0; i < x_num; i++) {
        for (size_t j = 0; j < y_num; j++) {
            for (size_t k = 0; k < z_num; k++) {
                gp_Pnt point(min_coor.X() + (i + 0.5) * dx,
                             min_coor.Y() + (j + 0.5) * dx,
                             min_coor.Z() + (k + 0.5) * dx);
                if (contain(point)) {
                    particles.emplace_back(point.X(), point.Y(), point.Z());
                }
                if (verbose) {
                    ++iter;
                    percent = (double)iter / total_num;
                    elapsed = T.elapsed();
                    if (iter % interval == 0) {
                        iter_per_second =
                            interval /
                            (t.elapsed() + std::numeric_limits<double>::min());
                        t.reset();
                    }
                    std::cout
                        << "\r" << std::setprecision(2)
                        << "Progress: " << percent * 100 << "%, "
                        << std::setprecision(1) << "Elapsed: " << elapsed
                        << "s, "
                        << "Estimated: "
                        << elapsed /
                               (percent + std::numeric_limits<double>::min()) *
                               (1 - percent)
                        << "s, " << iter_per_second << "it/s, "
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
                  << "Particle number: " << particles.size() << std::endl;
        std::cout << std::setprecision(-1);
        std::cout << ANSI_RESET_COLOR << std::endl;
    }
}

/*
void Model::fill_with_particle_mpi(const double _dx, const bool verbose)
{
    // Initialize MPI
    MPI_Init(NULL, NULL);

    // Get the total number of MPI processes and the rank of the current process
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the max and min coordinate of the model
    auto [max_coor, min_coor] = get_max_min_coor();

    // Calculate the number of particles in each direction
    auto x_num = (size_t)((max_coor.X() - min_coor.X()) / dx + 1.5);
    auto y_num = (size_t)((max_coor.Y() - min_coor.Y()) / dx + 1.5);
    auto z_num = (size_t)((max_coor.Z() - min_coor.Z()) / dx + 1.5);

    if (verbose && world_rank == 0) {
        // Print information only from the master process
        std::cout << "Filling " << name << "..." << std::endl;
        std::cout << "Max coordinate: " << max_coor.X() << ", " << max_coor.Y()
<< ", " << max_coor.Z() << std::endl; std::cout << "Min coordinate: " <<
min_coor.X() << ", " << min_coor.Y() << ", " << min_coor.Z() << std::endl;
        std::cout << "Total number: " << x_num * y_num * z_num << std::endl;
        std::cout << "Number in each direction: " << x_num << ", " << y_num <<
", " << z_num << std::endl; std::cout << std::fixed;
    }

    // Distribute the loop iterations among MPI processes
    size_t total_num = x_num * y_num * z_num;
    size_t num_iterations_per_process = total_num / world_size;
    size_t start_index = world_rank * num_iterations_per_process;
    size_t end_index = (world_rank == world_size - 1) ? total_num : start_index
+ num_iterations_per_process;

    // Fill the model with particles
    size_t iter = 0, interval = 1e2;
    double percent, elapsed, iter_per_second = 0;
    Timer T, t;
    particles.reserve(num_iterations_per_process);

    auto unflatten_index = [&](size_t flat_index, size_t& i, size_t& j, size_t&
k) { i = flat_index % x_num; j = (flat_index / x_num) % y_num; k = flat_index /
(x_num * y_num);
    };

    for (size_t idx = start_index; idx < end_index; ++idx) {
        size_t i, j, k;
        unflatten_index(idx, i, j, k);

        gp_Pnt point(
            min_coor.X() + (i + 0.5) * dx,
            min_coor.Y() + (j + 0.5) * dx,
            min_coor.Z() + (k + 0.5) * dx);

        if (contain(point)) {
            particles.emplace_back(point.X(), point.Y(), point.Z());
        }

        if (verbose) {
            ++iter;
            percent = (double)iter / total_num;
            elapsed = T.elapsed();
            if (iter % interval == 0) {
                iter_per_second = interval / (t.elapsed() +
std::numeric_limits<double>::min()); t.reset();
            }

            // Gather the progress information from all processes to the master
process MPI_Gather(&percent, 1, MPI_DOUBLE, NULL, 0, MPI_DOUBLE, 0,
MPI_COMM_WORLD);

            if (world_rank == 0) {
                // Print progress information only from the master process
                std::cout << "\r"
                          << std::setprecision(2)
                          << "Progress: " << percent * 100 << "%, "
                          << std::setprecision(1)
                          << "Elapsed: " << elapsed << "s, "
                          << "Estimated: " << elapsed / (percent +
std::numeric_limits<double>::min()) * (1 - percent) << "s, "
                          << iter_per_second << "it/s, "
                          << "Particle number: " << particles.size();
                std::cout.flush();
            }
        }
    }

    // Finalize MPI
    MPI_Finalize();
} */
