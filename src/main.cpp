#include <iostream>
#include <thread>
#include <map>
#include <cmath>

#include "universe.hpp"
#include "cell.hpp"
#include "animator.hpp"

using namespace std::chrono_literals;

void seedUniverse(Universe* universe, const std::vector<std::pair<int, int>>& seed) {
    for (const std::pair<int, int>& p: seed) {
        universe->makeCellAlive(p.first, p.second);
    }
}

void visualizeUniverse(Universe* universe, size_t time_steps) {
    std::unique_ptr<Animator> animator = std::make_unique<CenterAutoPanAnimator>(100ms);
    animator->animate(universe, time_steps);
}

int main(int argc, char** argv) {
    std::map<std::string, std::vector<std::pair<int, int>>> pattern_seed {
        {"toad", {{2,2}, {2,3}, {2,4}, {3,1}, {3,2}, {3,3}}},
        {"bee_hive", {{2,1}, {1,2}, {1,3}, {2,4}, {3,2}, {3,3}}},
        {"glider", {{2, 1}, {3, 2}, {3, 3}, {2, 3}, {1, 3}}},
        {"switch_engine", {
                              {1, 1}, {1, 2}, {1, 3}, {1, 5},
                              {2, 1}, {3, 4}, {3, 5},
                              {4, 2}, {4, 3}, {4, 5},
                              {5, 1}, {5, 3}, {5, 5},
                          }
        },
        {"switch_engine_2", {
                              {3, 3}, {3, 5},
                              {4, 2},
                              {5, 3}, {5, 6},
                              {6, 5}, {6, 6}, {6, 7},
                              {4, 15}, {4, 16}, // block
                              {5, 15}, {5, 16}
                          }
        },
        {"gosper_glider", {
                              {5, 1}, {5, 2}, {6, 1}, {6, 2},
                              {3, 13}, {3, 14}, {4, 12}, {4, 16}, {5, 11}, {5, 17}, {6, 11}, {6, 15}, {6, 17}, {6, 18},
                              {7, 11}, {7, 17}, {8, 12}, {8, 16}, {9, 13}, {9, 14},
                              {1, 25}, {2, 23}, {2, 25}, {3, 21}, {3, 22}, {4, 21}, {4, 22}, {5, 21}, {5, 22}, {6, 23},
                              {6, 25}, {7, 25},
                              {3, 35}, {3, 36}, {4, 35}, {4, 36},
                          }
        }
    };
    size_t rows = static_cast<size_t>(pow(2, 32));
    size_t cols = static_cast<size_t>(pow(2, 32));
    std::unique_ptr<Universe> universe = std::make_unique<SparseUniverseV2>(rows, cols);
    size_t time_steps = 350;
    if (argc > 1) {
        seedUniverse(universe.get(), pattern_seed.at(argv[1]));
        time_steps = argc > 2 ? std::stoi(argv[2]) : time_steps ;
    }
    else {
        seedUniverse(universe.get(), pattern_seed["gosper_glider"]);
    }
    visualizeUniverse(universe.get(), time_steps);
    universe->save("universe");
    return 0;
}
