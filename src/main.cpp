#include <iostream>

#include "universe.hpp"
#include "cell.hpp"

int main() {
    std::unique_ptr<Universe> universe = std::make_unique<DenseUniverse>(6, 6);
    std::vector<std::pair<int, int>> toad = {{2,2}, {2,3}, {2,4}, {3,1}, {3,2}, {3,3}};
    std::vector<std::pair<int, int>> bee_hive = {{2,1}, {1,2}, {1,3}, {2,4}, {3,2}, {3,3}};
    for (const std::pair<int, int>& p: bee_hive) {
        universe->cell(p.first, p.second)->makeAlive();
    }
    for (int i = 0; i < 10; ++i) {
        for (int row = 0; row < universe->rowCount(); row++) {
            for (int col = 0; col < universe->colCount(); col++) {
                universe->cell(row, col)->isAlive() ? std::cout << "1" : std::cout << "0";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        universe->advance();
    }
    return 0;
}
