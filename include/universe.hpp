#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <filesystem>
#include <vector>
#include <memory>
#include <set>
#include <unordered_map>
#include <optional>

#include "cell.hpp"

struct UniverseFileData {
    size_t rows;
    size_t cols;
    std::vector<std::pair<size_t, size_t>> alive_cells_pos;
};

// defines the interface for a Universe of Cells
class Universe {
    public:
        Universe(size_t rows, size_t cols);
        Universe(const std::filesystem::path& file_path);
        virtual void advance() = 0;
        virtual bool isCellAlive(size_t row, size_t col) = 0;
        virtual void makeCellAlive(size_t row, size_t col) = 0;
        virtual void makeCellDead(size_t row, size_t col) = 0;
        virtual std::vector<std::pair<size_t, size_t>> getAliveCellsPos() const = 0;
        virtual void save(const std::filesystem::path& file_path) const;
        virtual void load(const std::filesystem::path& file_path) = 0;
        size_t rowCount() const { return m_rows; }
        size_t colCount() const { return m_cols; }
        virtual ~Universe() {};
    protected:
        UniverseFileData parseFile(const std::filesystem::path& file_path);
        std::array<std::optional<std::pair<size_t, size_t>>, 8> getNeighborsPos(size_t row, size_t col);
        size_t m_rows;
        size_t m_cols;
        std::array<std::optional<std::pair<size_t, size_t>>, 8> m_neighbor_pos;
};

// keeps all Cells in memory
class DenseUniverse: public Universe {
    public:
        DenseUniverse(size_t rows, size_t cols);
        DenseUniverse(const std::filesystem::path& file_path);
        void advance() override;
        bool isCellAlive(size_t row, size_t col) override;
        void makeCellAlive(size_t row, size_t col) override;
        void makeCellDead(size_t row, size_t col) override;
        std::vector<std::pair<size_t, size_t>> getAliveCellsPos() const override;
        void save(const std::filesystem::path& file_path) const override;
        void load(const std::filesystem::path& file_path) override;
    private:
        void initCells();
        std::vector<Cell*> getNeighbors(const Cell& cell);
        std::vector<std::vector<Cell>>& getCurrentGrid();
        const std::vector<std::vector<Cell>>& getCurrentGrid() const;
        std::vector<std::vector<Cell>> m_cell_grid_1;
        std::vector<std::vector<Cell>> m_cell_grid_2;
        bool m_grid_1_is_current{true};
};

// keeps only alive Cells in memory
class SparseUniverse: public Universe {
    public:
        SparseUniverse(size_t rows, size_t cols);
        SparseUniverse(const std::filesystem::path& file_path);
        virtual void advance() override;
        virtual bool isCellAlive(size_t row, size_t col) override;
        virtual void makeCellAlive(size_t row, size_t col) override;
        virtual void makeCellDead(size_t row, size_t col) override;
        std::vector<std::pair<size_t, size_t>> getAliveCellsPos() const = 0;
        void save(const std::filesystem::path& file_path) const;
        void load(const std::filesystem::path& file_path);
    protected:
        virtual std::vector<Cell*> getAliveCells() = 0;
        virtual Cell* findAliveCellByPos(size_t row, size_t col) = 0;
        virtual void makeAndInsertNextAliveCell(size_t row, size_t col) = 0;
        virtual void makeAndInsertAliveCell(size_t row, size_t col) = 0;
        virtual void deleteCell(size_t row, size_t col) = 0;
        virtual void swapBuffers() = 0;
        virtual void clearBuffer() = 0;
        virtual void clearNextBuffer() = 0;
};

class SparseUniverseV1: public SparseUniverse {
    public:
        SparseUniverseV1(size_t rows, size_t cols);
        SparseUniverseV1(const std::filesystem::path& file_path);
        void advance() override;
        bool isCellAlive(size_t row, size_t col) override;
        void makeCellAlive(size_t row, size_t col) override;
        void makeCellDead(size_t row, size_t col) override;
        std::vector<std::pair<size_t, size_t>> getAliveCellsPos() const override;
        void save(const std::filesystem::path& file_path) const override;
        void load(const std::filesystem::path& file_path) override;
    private:
        std::vector<Cell*> getAliveCells() override;
        Cell* findAliveCellByPos(size_t row, size_t col) override;
        std::set<std::unique_ptr<Cell>>::iterator findAliveCellIterByPos(size_t row, size_t col);
        void makeAndInsertNextAliveCell(size_t row, size_t col) override;
        void makeAndInsertAliveCell(size_t row, size_t col) override;
        void deleteCell(size_t row, size_t col) override;
        void swapBuffers() override;
        void clearBuffer() override;
        void clearNextBuffer() override;
        std::set<std::unique_ptr<Cell>> m_alive_cells;
        std::set<std::unique_ptr<Cell>> m_next_alive_cells;
};

class SparseUniverseV2: public SparseUniverse {
    public:
        SparseUniverseV2(size_t rows, size_t cols);
        SparseUniverseV2(const std::filesystem::path& file_path);
        void advance() override;
        bool isCellAlive(size_t row, size_t col) override;
        void makeCellAlive(size_t row, size_t col) override;
        void makeCellDead(size_t row, size_t col) override;
        std::vector<std::pair<size_t, size_t>> getAliveCellsPos() const override;
        void save(const std::filesystem::path& file_path) const override;
        void load(const std::filesystem::path& file_path) override;
    private:
        std::vector<Cell*> getAliveCells() override;
        Cell* findAliveCellByPos(size_t row, size_t col) override;
        void makeAndInsertNextAliveCell(size_t row, size_t col) override;
        void makeAndInsertAliveCell(size_t row, size_t col) override;
        void deleteCell(size_t row, size_t col) override;
        void swapBuffers() override;
        void clearBuffer() override;
        void clearNextBuffer() override;
        std::unordered_map<size_t, Cell> m_alive_cells;
        std::unordered_map<size_t, Cell> m_next_alive_cells;
};

template <size_t Rows, size_t Cols>
class DenseUniverseV2: public Universe {
    public:
        DenseUniverseV2();
        DenseUniverseV2(const std::filesystem::path& file_path);
        void advance() override;
        bool isCellAlive(size_t row, size_t col) override;
        void makeCellAlive(size_t row, size_t col) override;
        void makeCellDead(size_t row, size_t col) override;
        // the only dynamic alloc part
        std::vector<std::pair<size_t, size_t>> getAliveCellsPos() const override;
        void save(const std::filesystem::path& file_path) const override;
        void load(const std::filesystem::path& file_path) override;
    private:
        void initCells();
        std::array<std::optional<Cell*>, 8> getNeighbors(const Cell& cell);
        std::array<std::array<Cell, Cols>, Rows>& getCurrentGrid();
        const std::array<std::array<Cell, Cols>, Rows>& getCurrentGrid() const;
        std::array<std::array<Cell, Cols>, Rows> m_cell_grid_1;
        std::array<std::array<Cell, Cols>, Rows> m_cell_grid_2;
        std::array<std::optional<Cell*>, 8> m_neighbors;
        bool m_grid_1_is_current{true};
};

// no dynamic allocation in this universe
template <size_t Rows, size_t Cols>
DenseUniverseV2<Rows, Cols>::DenseUniverseV2(): Universe(Rows, Cols) {
    initCells();
}

template <size_t Rows, size_t Cols>
std::array<std::array<Cell, Cols>, Rows>& DenseUniverseV2<Rows, Cols>::getCurrentGrid() {
    return m_grid_1_is_current ? m_cell_grid_1 : m_cell_grid_2;
}

template <size_t Rows, size_t Cols>
const std::array<std::array<Cell, Cols>, Rows>& DenseUniverseV2<Rows, Cols>::getCurrentGrid() const {
    return m_grid_1_is_current ? m_cell_grid_1 : m_cell_grid_2;
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::initCells() {
    for (size_t row = 0; row < m_rows; ++row) {
        for (size_t col = 0; col < m_cols; ++col) {
            m_cell_grid_1[row][col] = Cell(row, col, m_cols * row + col, false);
            m_cell_grid_2[row][col] = Cell(row, col, m_cols * row + col, false);
        }
    }
}

template <size_t Rows, size_t Cols>
DenseUniverseV2<Rows, Cols>::DenseUniverseV2(const std::filesystem::path& file_path): Universe(file_path) {
    auto fdata = Universe::parseFile(file_path);
    m_rows = fdata.rows;
    m_cols = fdata.cols;
    initCells();
    auto& grid = getCurrentGrid();
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        grid[p.first][p.second].makeAlive();
    }
}

template <size_t Rows, size_t Cols>
std::array<std::optional<Cell*>, 8> DenseUniverseV2<Rows, Cols>::getNeighbors(const Cell& cell) {
    std::vector<Cell*> neighbors;
    auto& grid = getCurrentGrid();
    size_t nei_idx = 0;
    for (const auto& pos: getNeighborsPos(cell.row(), cell.col())) {
        if (pos.has_value()) {
            const auto& [row, col] = pos.value();
            m_neighbors[nei_idx++] = &grid[row][col];
        }
        else {
            m_neighbors[nei_idx++] = std::nullopt;
        }
    }
    return m_neighbors;
}

template <size_t Rows, size_t Cols>
bool DenseUniverseV2<Rows, Cols>::isCellAlive(size_t row, size_t col) {
    const auto& grid = getCurrentGrid();
    return grid[row][col].isAlive();
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::makeCellAlive(size_t row, size_t col) {
    auto& grid = getCurrentGrid();
    grid[row][col].makeAlive();
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::makeCellDead(size_t row, size_t col) {
    auto& grid = getCurrentGrid();
    grid[row][col].makeDead();
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::advance() {
    const auto& grid = getCurrentGrid();
    auto& next_grid = m_grid_1_is_current ? m_cell_grid_2 : m_cell_grid_1;

    for (size_t row = 0; row < m_rows; row++) {
        for (size_t col = 0; col < m_cols; col++) {
            const Cell& cell = grid[row][col];
            size_t alive_count = 0;
            for (auto& neighbor: getNeighbors(cell)) {
                if (!neighbor.has_value()) {
                    continue;
                }
                alive_count = neighbor.value()->isAlive() ? alive_count + 1: alive_count;
            }
            if (cell.isAlive()) {
                if (alive_count < 2 || alive_count > 3) {
                    next_grid[row][col].makeDead();
                }
                else {
                    next_grid[row][col].makeAlive();
                }
            }
            else {
                if (alive_count == 3) {
                    next_grid[row][col].makeAlive();
                }
                else {
                    next_grid[row][col].makeDead();
                }
            }
        }
    }

    m_grid_1_is_current = !m_grid_1_is_current;
}

template <size_t Rows, size_t Cols>
std::vector<std::pair<size_t, size_t>> DenseUniverseV2<Rows, Cols>::getAliveCellsPos() const {
    std::vector<std::pair<size_t, size_t>> alive_pos;
    const auto& grid = getCurrentGrid();
    for (size_t row = 0; row < m_rows; row++) {
        for (size_t col = 0; col < m_cols; col++) {
            if (grid[row][col].isAlive()) {
                alive_pos.push_back({row, col});
            }
        }
    }
    return alive_pos;
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::save(const std::filesystem::path& file_path) const {
    Universe::save(file_path);
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::load(const std::filesystem::path& file_path) {
    auto& grid = getCurrentGrid();
    for (size_t row = 0; row < m_rows; ++row) {
        for (size_t col = 0; col < m_cols; ++col) {
            grid[row][col].makeDead();
        }
    }
    auto fdata = Universe::parseFile(file_path);
    if (fdata.rows != m_rows || fdata.cols != m_cols) {
        throw std::runtime_error("Cannot load a universe with a mismatched size");
    }
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        grid[p.first][p.second].makeAlive();
    }
}

#endif
