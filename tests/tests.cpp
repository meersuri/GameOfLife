#include <gtest/gtest.h>

#include "universe.hpp"
#include "cell.hpp"

void testUniverseStartsDead(std::unique_ptr<Universe>&& universe) {
    for (size_t row = 0; row < universe->rowCount(); ++row) {
        for (size_t col = 0; col < universe->colCount(); ++col) {
            ASSERT_FALSE(universe->isCellAlive(row, col));
        }
    }
}

void testMakeCellAlive(std::unique_ptr<Universe>&& universe) {
    universe->makeCellAlive(0, 0);
    ASSERT_TRUE(universe->isCellAlive(0, 0));
}

void testMakeCellDead(std::unique_ptr<Universe>&& universe) {
    universe->makeCellAlive(0, 0);
    universe->makeCellDead(0, 0);
    ASSERT_FALSE(universe->isCellAlive(0, 0));
}

template <typename UnivT>
void testCellComesAlive() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{0, 0}, {2, 2}, {2, 0}},
        {{1, 0}, {2, 1}, {1, 2}},
        {{0, 0}, {0, 2}, {2, 1}},
        {{1, 0}, {1, 2}, {0, 1}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->advance();
        ASSERT_TRUE(universe->isCellAlive(1, 1));
    }
}

template <typename UnivT>
void testCellStaysDead() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{0, 0}, {2, 2}, {2, 0}, {2, 1}},
        {{1, 0}, {2, 1}},
        {{0, 0}},
        {{1, 0}, {1, 2}, {0, 1}, {2, 2}, {0, 0}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->advance();
        ASSERT_FALSE(universe->isCellAlive(1, 1));
    }
}

template <typename UnivT>
void testCellDies() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{0, 0}, {2, 2}, {2, 0}, {2, 1}},
        {{0, 0}},
        {{2, 2}},
        {{1, 0}, {1, 2}, {0, 1}, {2, 2}, {0, 0}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->makeCellAlive(1, 1);
        universe->advance();
        ASSERT_FALSE(universe->isCellAlive(1, 1));
    }
}

template <typename UnivT>
void testCellStaysAlive() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{0, 1}, {2, 2}, {2, 0}},
        {{0, 0}, {0, 1}, {2, 1}},
        {{2, 2}, {0,0}},
        {{1, 0}, {1, 2}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->makeCellAlive(1, 1);
        universe->advance();
        ASSERT_TRUE(universe->isCellAlive(1, 1));
    }
}

template <typename UnivT>
void testSaveLoad() {
    std::vector<std::pair<size_t, size_t>> alive_cells_pos = {
        {{0, 1}, {2, 2}, {2, 0}, {1, 3}, {2, 3}, {1, 1}}
    };
    auto universe = std::make_unique<UnivT>(3, 4);
    for (const std::pair<int, int>& pos: alive_cells_pos) {
        universe->makeCellAlive(pos.first, pos.second);
    }
    universe->save("test_universe");
    auto new_universe = std::make_unique<UnivT>(3, 4);
    new_universe->load("test_universe"); // TODO delete this file at the end of the test
    for (const std::pair<int, int>& pos: alive_cells_pos) {
        ASSERT_TRUE(new_universe->isCellAlive(pos.first, pos.second));
    }
}

template <typename UnivT>
void testCreateUniverseFromFile() {
    std::vector<std::pair<size_t, size_t>> alive_cells_pos = {
        {{0, 1}, {2, 2}, {2, 0}, {1, 3}, {2, 3}, {1, 1}}
    };
    auto universe = std::make_unique<UnivT>(3, 4);
    for (const std::pair<int, int>& pos: alive_cells_pos) {
        universe->makeCellAlive(pos.first, pos.second);
    }
    universe->save("test_universe");// TODO delete this file at the end of the test
    auto new_universe = std::make_unique<UnivT>("test_universe");
    ASSERT_EQ(new_universe->rowCount(), 3);
    ASSERT_EQ(new_universe->colCount(), 4);
    for (const std::pair<int, int>& pos: alive_cells_pos) {
        ASSERT_TRUE(new_universe->isCellAlive(pos.first, pos.second));
    }
}

// DenseUniverse tests
TEST(DenseUniverseTests, UniverseStartsDead) {
    testUniverseStartsDead(std::make_unique<DenseUniverse>(3, 4));
}

TEST(DenseUniverseTests, makeCellAlive) {
    testMakeCellAlive(std::make_unique<DenseUniverse>(1, 1));
}

TEST(DenseUniverseTests, makeCellDead) {
    testMakeCellDead(std::make_unique<DenseUniverse>(1, 1));
}

TEST(DenseUniverseTests, cellComesAlive) {
    testCellComesAlive<DenseUniverse>();
}

TEST(DenseUniverseTests, cellStaysDead) {
    testCellStaysDead<DenseUniverse>();
}

TEST(DenseUniverseTests, cellDies) {
    testCellDies<DenseUniverse>();
}

TEST(DenseUniverseTests, cellStaysAlive) {
    testCellStaysAlive<DenseUniverse>();
}

TEST(DenseUniverseTests, saveAndLoad) {
    testSaveLoad<DenseUniverse>();
}

TEST(DenseUniverseTests, createFromFile) {
    testCreateUniverseFromFile<DenseUniverse>();
}

// SparseUniverse tests
TEST(SparseUniverseTests, UniverseStartsDead) {
    testUniverseStartsDead(std::make_unique<SparseUniverse>(3, 4));
}

TEST(SparseUniverseTests, makeCellAlive) {
    testMakeCellAlive(std::make_unique<SparseUniverse>(1, 1));
}

TEST(SparseUniverseTests, makeCellDead) {
    testMakeCellDead(std::make_unique<SparseUniverse>(1, 1));
}

TEST(SparseUniverseTests, cellComesAlive) {
    testCellComesAlive<SparseUniverse>();
}

TEST(SparseUniverseTests, cellStaysDead) {
    testCellStaysDead<SparseUniverse>();
}

TEST(SparseUniverseTests, cellDies) {
    testCellDies<SparseUniverse>();
}

TEST(SparseUniverseTests, cellStaysAlive) {
    testCellStaysAlive<SparseUniverse>();
}

TEST(SparseUniverseTests, saveAndLoad) {
    testSaveLoad<SparseUniverse>();
}

TEST(SparseUniverseTests, createFromFile) {
    testCreateUniverseFromFile<SparseUniverse>();
}
