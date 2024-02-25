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

// 8 neighbors: 8C3 combinations
template <typename UnivT>
void testNonEdgeCellComesAlive() {
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

// 5 neighbors: 5C3 combinations
template <typename UnivT>
void testEdgeCellComesAlive() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{0, 0}, {2, 0}, {1, 1}},
        {{0, 1}, {2, 1}, {1, 1}},
        {{0, 1}, {1, 1}, {2, 1}},
        {{2, 0}, {2, 1}, {1, 1}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->advance();
        ASSERT_TRUE(universe->isCellAlive(1, 0));
    }
}

// 3 neighbors: 1 combination
template <typename UnivT>
void testCornerCellComesAlive() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{1, 0}, {1, 1}, {0, 1}},
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->advance();
        ASSERT_TRUE(universe->isCellAlive(0, 0));
    }
}

template <typename UnivT>
void testNonEdgeCellStaysDead() {
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
void testEdgeCellStaysDead() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{0, 0}, {0, 2}, {1, 1}, {1, 2}},
        {{0, 0}, {1, 1}},
        {{0, 2}},
        {{0, 0}, {0, 2}, {1, 0}, {1, 1}, {1, 2}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->advance();
        ASSERT_FALSE(universe->isCellAlive(0, 1));
    }
}

template <typename UnivT>
void testCornerCellStaysDead() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{1, 1}, {1, 0}},
        {{0, 1}, {1, 2}},
        {{0, 1}, {1, 1}},
        {{0, 1}},
        {{1, 2}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->advance();
        ASSERT_FALSE(universe->isCellAlive(0, 2));
    }
}

template <typename UnivT>
void testNonEdgeCellDies() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{0, 0}, {2, 2}, {2, 0}, {2, 1}},
        {{0, 0}},
        {{2, 2}},
        {},
        {{1, 0}, {1, 2}, {0, 1}, {2, 2}, {0, 0}},
        {{1, 0}, {1, 2}, {0, 1}, {2, 2}, {0, 0}, {2, 1}}
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
void testEdgeCellDies() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{1, 0}, {2, 2}, {2, 0}, {1, 2}},
        {{1, 0}},
        {{2, 2}},
        {},
        {{1, 1}, {2, 0}, {1, 0}, {2, 2}, {1, 2}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->makeCellAlive(2, 1);
        universe->advance();
        ASSERT_FALSE(universe->isCellAlive(2, 1));
    }
}

template <typename UnivT>
void testCornerCellDies() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{1, 1}},
        {{0, 1}},
        {{1, 2}},
        {}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->makeCellAlive(0, 2);
        universe->advance();
        ASSERT_FALSE(universe->isCellAlive(0, 2));
    }
}

template <typename UnivT>
void testNonEdgeCellStaysAlive() {
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
void testEdgeCellStaysAlive() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{1, 1}, {0, 2}, {2, 2}},
        {{0, 1}, {1, 1}, {2, 1}},
        {{2, 2}, {0, 1}},
        {{1, 1}, {0, 2}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->makeCellAlive(1, 2);
        universe->advance();
        ASSERT_TRUE(universe->isCellAlive(1, 2));
    }
}

template <typename UnivT>
void testCornerCellStaysAlive() {
    std::vector<std::vector<std::pair<int, int>>> alive_neighbors = {
        {{1, 2}, {2, 1}, {1, 1}},
        {{2, 1}, {1, 1}},
        {{1, 2}, {1, 1}},
        {{1, 2}, {2, 1}}
    };
    for (const std::vector<std::pair<int, int>>& nei: alive_neighbors) {
        auto universe = std::make_unique<UnivT>(3, 3);
        for (const std::pair<int, int>& pos: nei) {
            universe->makeCellAlive(pos.first, pos.second);
        }
        universe->makeCellAlive(2, 2);
        universe->advance();
        ASSERT_TRUE(universe->isCellAlive(2, 2));
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
    for (const std::string& save_path: {"test_universe", "test_universe.univ"}) {
        universe->save(save_path);
        auto new_universe = std::make_unique<UnivT>(3, 4);
        new_universe->load("test_universe.univ"); // TODO delete this file at the end of the test
        for (const std::pair<int, int>& pos: alive_cells_pos) {
            ASSERT_TRUE(new_universe->isCellAlive(pos.first, pos.second));
        }
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
    universe->save("test_universe.univ");// TODO delete this file at the end of the test
    auto new_universe = std::make_unique<UnivT>("test_universe.univ");
    ASSERT_EQ(new_universe->rowCount(), 3);
    ASSERT_EQ(new_universe->colCount(), 4);
    for (const std::pair<int, int>& pos: alive_cells_pos) {
        ASSERT_TRUE(new_universe->isCellAlive(pos.first, pos.second));
    }
}

// DenseUniverseV1 tests
TEST(DenseUniverseV1Tests, UniverseStartsDead) {
    testUniverseStartsDead(std::make_unique<DenseUniverseV1>(3, 4));
}

TEST(DenseUniverseV1Tests, makeCellAlive) {
    testMakeCellAlive(std::make_unique<DenseUniverseV1>(1, 1));
}

TEST(DenseUniverseV1Tests, makeCellDead) {
    testMakeCellDead(std::make_unique<DenseUniverseV1>(1, 1));
}

TEST(DenseUniverseV1Tests, cellComesAlive) {
    testNonEdgeCellComesAlive<DenseUniverseV1>();
    testEdgeCellComesAlive<DenseUniverseV1>();
    testCornerCellComesAlive<DenseUniverseV1>();
}

TEST(DenseUniverseV1Tests, cellStaysDead) {
    testNonEdgeCellStaysDead<DenseUniverseV1>();
    testEdgeCellStaysDead<DenseUniverseV1>();
    testCornerCellStaysDead<DenseUniverseV1>();
}

TEST(DenseUniverseV1Tests, cellDies) {
    testNonEdgeCellDies<DenseUniverseV1>();
    testEdgeCellDies<DenseUniverseV1>();
    testCornerCellDies<DenseUniverseV1>();
}

TEST(DenseUniverseV1Tests, cellStaysAlive) {
    testNonEdgeCellStaysAlive<DenseUniverseV1>();
    testEdgeCellStaysAlive<DenseUniverseV1>();
    testCornerCellStaysAlive<DenseUniverseV1>();
}

TEST(DenseUniverseV1Tests, saveAndLoad) {
    testSaveLoad<DenseUniverseV1>();
}

TEST(DenseUniverseV1Tests, createFromFile) {
    testCreateUniverseFromFile<DenseUniverseV1>();
}

// SparseUniverse tests
TEST(SparseUniverseTests, UniverseStartsDead) {
    testUniverseStartsDead(std::make_unique<SparseUniverseV1>(3, 4));
}

TEST(SparseUniverseV1Tests, makeCellAlive) {
    testMakeCellAlive(std::make_unique<SparseUniverseV1>(1, 1));
}

TEST(SparseUniverseV1Tests, makeCellDead) {
    testMakeCellDead(std::make_unique<SparseUniverseV1>(1, 1));
}

TEST(SparseUniverseV1Tests, cellComesAlive) {
    testNonEdgeCellComesAlive<SparseUniverseV1>();
    testEdgeCellComesAlive<SparseUniverseV1>();
    testCornerCellComesAlive<SparseUniverseV1>();
}

TEST(SparseUniverseV1Tests, cellStaysDead) {
    testNonEdgeCellStaysDead<SparseUniverseV1>();
    testEdgeCellStaysDead<SparseUniverseV1>();
    testCornerCellStaysDead<SparseUniverseV1>();
}

TEST(SparseUniverseV1Tests, cellDies) {
    testNonEdgeCellDies<SparseUniverseV1>();
    testEdgeCellDies<SparseUniverseV1>();
    testCornerCellDies<SparseUniverseV1>();
}

TEST(SparseUniverseV1Tests, cellStaysAlive) {
    testNonEdgeCellStaysAlive<SparseUniverseV1>();
    testEdgeCellStaysAlive<SparseUniverseV1>();
    testCornerCellStaysAlive<SparseUniverseV1>();
}

TEST(SparseUniverseV1Tests, saveAndLoad) {
    testSaveLoad<SparseUniverseV1>();
}

TEST(SparseUniverseV1Tests, createFromFile) {
    testCreateUniverseFromFile<SparseUniverseV1>();
}


// SparseUniverseV2 tests
TEST(SparseUniverseV2Tests, UniverseStartsDead) {
    testUniverseStartsDead(std::make_unique<SparseUniverseV2>(3, 4));
}

TEST(SparseUniverseV2Tests, makeCellAlive) {
    testMakeCellAlive(std::make_unique<SparseUniverseV2>(1, 1));
}

TEST(SparseUniverseV2Tests, makeCellDead) {
    testMakeCellDead(std::make_unique<SparseUniverseV2>(1, 1));
}

TEST(SparseUniverseV2Tests, cellComesAlive) {
    testNonEdgeCellComesAlive<SparseUniverseV2>();
    testEdgeCellComesAlive<SparseUniverseV2>();
    testCornerCellComesAlive<SparseUniverseV2>();
}

TEST(SparseUniverseV2Tests, cellStaysDead) {
    testNonEdgeCellStaysDead<SparseUniverseV2>();
    testEdgeCellStaysDead<SparseUniverseV2>();
    testCornerCellStaysDead<SparseUniverseV2>();
}

TEST(SparseUniverseV2Tests, cellDies) {
    testNonEdgeCellDies<SparseUniverseV2>();
    testEdgeCellDies<SparseUniverseV2>();
    testCornerCellDies<SparseUniverseV2>();
}

TEST(SparseUniverseV2Tests, cellStaysAlive) {
    testNonEdgeCellStaysAlive<SparseUniverseV2>();
    testEdgeCellStaysAlive<SparseUniverseV2>();
    testCornerCellStaysAlive<SparseUniverseV2>();
}

TEST(SparseUniverseV2Tests, saveAndLoad) {
    testSaveLoad<SparseUniverseV2>();
}

TEST(SparseUniverseV2Tests, createFromFile) {
    testCreateUniverseFromFile<SparseUniverseV2>();
}
