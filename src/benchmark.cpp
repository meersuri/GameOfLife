#include <chrono>
#include <iostream>

#include "universe.hpp"
#include "cell.hpp"

int main(int argc, char** argv) {
    std::unique_ptr<Universe> universe = std::make_unique<SparseUniverseV2>("gosper_glider.univ");
    size_t time_steps = 10000;
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < time_steps; ++i) {
        universe->advance();
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(end - start);
    std::cout << "Time to " << time_steps << " steps of Gosper's glider: " << duration.count() << " s\n";
    std::cout << "Alive cell count: " << universe->getAliveCellsPos().size() << '\n';
    return 0;
}
