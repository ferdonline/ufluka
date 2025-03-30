#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

// Simulation constants
constexpr int NUM_PARTICLES = 1000000;
constexpr int MAX_STEPS = 200;
constexpr double ABSORPTION_PROB = 0.1;
constexpr double SCATTER_PROB = 0.7;
constexpr double DOMAIN_SIZE = 40.0;     // cm
constexpr int GRID_SIZE = 400;           // resolution
constexpr double INITIAL_ENERGY = 800.0; // Initial energy per proton in MeV
constexpr double PROTON_MASS = 938.3;    // MeV/c^2
constexpr double ELECTRON_MASS = 0.511;  // MeV/c^2
constexpr double I_CONCRETE = 0.000286;  // Mean excitation energy of concrete in GeV
constexpr double Z_CONCRETE = 11;        // Effective atomic number of concrete
constexpr double BEAM_SPREAD = 0.3;      // cm (3 mm beam spot)
constexpr double ANGULAR_SPREAD = 0.001; // 1 mrad

// Energy deposition grid
double energy_grid[GRID_SIZE][GRID_SIZE]{};

// Random number generator
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> uniform_dist(0.0, 1.0);
std::normal_distribution<> beam_y_dist(DOMAIN_SIZE / 2.0, BEAM_SPREAD);
std::normal_distribution<> angle_dist(0.0, ANGULAR_SPREAD);

// Generate a random number between 0 and 1
double rand_uniform() { return uniform_dist(gen); }

// Simplified Bethe-Bloch energy loss (dE/dx) in MeV/cm
double bethe_bloch(double energy) {
    double beta = 1 - pow(PROTON_MASS / (energy + PROTON_MASS), 2);
    double dEdx = (0.15 * Z_CONCRETE / beta) * log((2 * ELECTRON_MASS * beta) / I_CONCRETE);
    return std::max(dEdx, 0.01); // Ensure positive energy loss
}

// Generate a random path length using proton interaction length
double random_path_length() { return -log(rand_uniform()) * 5.0; }

// Generate an energy-dependent scattering angle
double random_proton_angle(double energy) {
    double max_angle = std::min(0.2 * (INITIAL_ENERGY / energy), 0.4);
    return (rand_uniform() - 0.5) * max_angle;
}

int main() {
    std::ofstream file("energy_deposition.txt");

    for (int i = 0; i < NUM_PARTICLES; i++) {
        // Gaussian beam spread
        double x = 2.0;
        double y = beam_y_dist(gen);    // Initial beam particle position
        double theta = angle_dist(gen); // Initial beam divergence
        double energy = INITIAL_ENERGY;

        for (int step = 0; step < MAX_STEPS; step++) {
            if (energy <= 0)
                break;

            double path_length = random_path_length();
            x += path_length * cos(theta);
            y += path_length * sin(theta);

            // Lateral displacement due to multiple scattering
            x += (rand_uniform() - 0.5) * 0.5;
            y += (rand_uniform() - 0.5) * 0.5;

            // Check if proton leaves the domain
            if (x < 0 || x >= DOMAIN_SIZE || y < 0 || y >= DOMAIN_SIZE) {
                break;
            }

            // Convert position to grid indices
            int grid_x = static_cast<int>(x * GRID_SIZE / DOMAIN_SIZE);
            int grid_y = static_cast<int>(y * GRID_SIZE / DOMAIN_SIZE);

            // Compute energy loss
            double dE = bethe_bloch(energy) * path_length;
            dE = std::min(dE, energy);
            energy_grid[grid_y][grid_x] += dE;
            energy -= dE;

            // Determine interaction type
            double interaction = rand_uniform();
            if (interaction < ABSORPTION_PROB) {
                energy_grid[grid_y][grid_x] += energy; // Deposit remaining energy before absorption
                break;
            } else if (interaction < ABSORPTION_PROB + SCATTER_PROB) {
                theta += random_proton_angle(energy);
            }
        }
    }

    // Save energy grid to file
    for (const auto &row : energy_grid) {
        for (double val : row) {
            file << val << " ";
        }
        file << "\n";
    }
    file.close();

    return 0;
}
