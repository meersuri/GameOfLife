#include <iostream>
#include <thread>

#include "universe.hpp"
#include "cell.hpp"
#include "painter.hpp"

using namespace std::chrono_literals;

int main() {
    std::unique_ptr<Universe> universe = std::make_unique<DenseUniverse>(20, 20);
    std::vector<std::pair<int, int>> toad = {{2,2}, {2,3}, {2,4}, {3,1}, {3,2}, {3,3}};
    std::vector<std::pair<int, int>> bee_hive = {{2,1}, {1,2}, {1,3}, {2,4}, {3,2}, {3,3}};
    std::vector<std::pair<int, int>> glider = {{2, 1}, {3, 2}, {3, 3}, {2, 3}, {1, 3}};
    for (const std::pair<int, int>& p: glider) {
        universe->cell(p.first, p.second)->makeAlive();
    }
    GridPainter painter(universe->rowCount(), universe->colCount());
    size_t time_steps = 50;
    painter.clear();
    for (int i = 0; i < time_steps; ++i) {
        for (int row = 0; row < universe->rowCount(); row++) {
            for (int col = 0; col < universe->colCount(); col++) {
                if (universe->cell(row, col)->isAlive()) {
                    painter.paint(row, col, '*', Color::green);
                }
                else {
                    painter.paint(row, col, ' ', Color::black);
                }
            }
        }
        universe->advance();
        std::this_thread::sleep_for(500ms);
        painter.shiftCursor(0, 0);
    }
    painter.shiftCursor(universe->rowCount() + 1, 1);
    return 0;
}
