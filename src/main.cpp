#include <iostream>
#include <thread>

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
    std::vector<std::pair<int, int>> toad = {{2,2}, {2,3}, {2,4}, {3,1}, {3,2}, {3,3}};
    std::vector<std::pair<int, int>> bee_hive = {{2,1}, {1,2}, {1,3}, {2,4}, {3,2}, {3,3}};
    std::vector<std::pair<int, int>> glider = {{2, 1}, {3, 2}, {3, 3}, {2, 3}, {1, 3}};
    std::unique_ptr<Universe> universe = std::make_unique<SparseUniverse>(20, 20);
    size_t time_steps = 50;
    if (argc > 1) {
        std::string pattern(argv[1]);
        if (pattern == "toad") {
            seedUniverse(universe.get(), toad);
            time_steps = 20;
        }
        else if (pattern == "bee_hive") {
            seedUniverse(universe.get(), bee_hive);
            time_steps = 10;
        }
        else {
            seedUniverse(universe.get(), glider);
        }
    }
    else {
        seedUniverse(universe.get(), glider);
    }
    visualizeUniverse(universe.get(), time_steps);
    return 0;
}
