#include <iostream>
#include <thread>
#include <map>

#include "universe.hpp"
#include "cell.hpp"
#include "painter.hpp"

using namespace std::chrono_literals;

void seedUniverse(Universe* universe, const std::vector<std::pair<int, int>>& seed) {
    for (const std::pair<int, int>& p: seed) {
        universe->makeCellAlive(p.first, p.second);
    }
}

void visualizeUniverse(Universe* universe, size_t time_steps) {
    GridPainter painter(universe->rowCount(), universe->colCount());
    painter.clear();
    for (int i = 0; i < time_steps; ++i) {
        for (const std::pair<int, int>& pos: universe->getAliveCellsPos()) {
            painter.paint(pos.first, pos.second, '*', Color::green);
        }
        universe->advance();
        std::this_thread::sleep_for(500ms);
        painter.clear();
        painter.shiftCursor(0, 0);
    }
    painter.shiftCursor(universe->rowCount() + 1, 1);
}

int main(int argc, char** argv) {
    std::map<std::string, std::vector<std::pair<int, int>>> pattern_seed {
        {"toad", {{2,2}, {2,3}, {2,4}, {3,1}, {3,2}, {3,3}}},
        {"bee_hive", {{2,1}, {1,2}, {1,3}, {2,4}, {3,2}, {3,3}}},
        {"glider", {{2, 1}, {3, 2}, {3, 3}, {2, 3}, {1, 3}}},
    };
    std::unique_ptr<Universe> universe = std::make_unique<SparseUniverse>(20, 20);
    size_t time_steps = 50;
    if (argc > 1) {
        seedUniverse(universe.get(), pattern_seed.at(argv[1]));
        time_steps = argc > 2 ? std::stoi(argv[2]) : time_steps ;
    }
    else {
        seedUniverse(universe.get(), pattern_seed["glider"]);
    }
    visualizeUniverse(universe.get(), time_steps);
    return 0;
}
