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
        std::array<std::optional<std::pair<size_t, size_t>>, 8>& getNeighborsPos(size_t row, size_t col);
        size_t m_rows;
        size_t m_cols;
        std::array<std::optional<std::pair<size_t, size_t>>, 8> m_neighbor_pos;
        std::array<std::optional<Cell*>, 8> m_neighbors;
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
    protected:
        virtual void initCells() = 0;
        virtual std::array<std::optional<Cell*>, 8>& getNeighbors(const Cell& cell) = 0;
        virtual Cell* getCurrentGridCell(size_t row, size_t col) = 0;
        virtual Cell const* getCurrentGridCell(size_t row, size_t col) const = 0;
        virtual Cell* getNextGridCell(size_t row, size_t col) = 0;
        bool m_grid_1_is_current{true};
};

class DenseUniverseV1: public DenseUniverse {
    public:
        DenseUniverseV1(size_t rows, size_t cols);
        DenseUniverseV1(const std::filesystem::path& file_path);
        void advance() override;
        bool isCellAlive(size_t row, size_t col) override;
        void makeCellAlive(size_t row, size_t col) override;
        void makeCellDead(size_t row, size_t col) override;
        std::vector<std::pair<size_t, size_t>> getAliveCellsPos() const override;
        void save(const std::filesystem::path& file_path) const override;
        void load(const std::filesystem::path& file_path) override;
    private:
        void initCells() override;
        std::array<std::optional<Cell*>, 8>& getNeighbors(const Cell& cell) override;
        Cell* getCurrentGridCell(size_t row, size_t col) override;
        Cell const* getCurrentGridCell(size_t row, size_t col) const override;
        Cell* getNextGridCell(size_t row, size_t col) override;
        std::vector<std::vector<Cell>> m_cell_grid_1;
        std::vector<std::vector<Cell>> m_cell_grid_2;
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
class DenseUniverseV2: public DenseUniverse {
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
        void initCells() override;
        std::array<std::optional<Cell*>, 8>& getNeighbors(const Cell& cell) override;
        Cell* getCurrentGridCell(size_t row, size_t col) override;
        Cell const* getCurrentGridCell(size_t row, size_t col) const override;
        Cell* getNextGridCell(size_t row, size_t col) override;
        std::array<std::array<Cell, Cols>, Rows> m_cell_grid_1;
        std::array<std::array<Cell, Cols>, Rows> m_cell_grid_2;
};

// no dynamic allocation in this universe
template <size_t Rows, size_t Cols>
DenseUniverseV2<Rows, Cols>::DenseUniverseV2(): DenseUniverse(Rows, Cols) {
    initCells();
}

template <size_t Rows, size_t Cols>
DenseUniverseV2<Rows, Cols>::DenseUniverseV2(const std::filesystem::path& file_path): DenseUniverse(file_path) {
    auto fdata = Universe::parseFile(file_path);
    m_rows = fdata.rows;
    m_cols = fdata.cols;
    initCells();
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        getCurrentGridCell(p.first, p.second)->makeAlive();
    }
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
Cell* DenseUniverseV2<Rows, Cols>::getCurrentGridCell(size_t row, size_t col) {
    return m_grid_1_is_current ? &m_cell_grid_1[row][col] : &m_cell_grid_2[row][col];
}

template <size_t Rows, size_t Cols>
Cell const* DenseUniverseV2<Rows, Cols>::getCurrentGridCell(size_t row, size_t col) const {
    return m_grid_1_is_current ? &m_cell_grid_1[row][col] : &m_cell_grid_2[row][col];
}

template <size_t Rows, size_t Cols>
Cell* DenseUniverseV2<Rows, Cols>::getNextGridCell(size_t row, size_t col) {
    return m_grid_1_is_current ? &m_cell_grid_2[row][col] : &m_cell_grid_1[row][col];
}

template <size_t Rows, size_t Cols>
std::array<std::optional<Cell*>, 8>& DenseUniverseV2<Rows, Cols>::getNeighbors(const Cell& cell) {
    return DenseUniverse::getNeighbors(cell);
}

template <size_t Rows, size_t Cols>
bool DenseUniverseV2<Rows, Cols>::isCellAlive(size_t row, size_t col) {
    return DenseUniverse::isCellAlive(row, col);
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::makeCellAlive(size_t row, size_t col) {
    DenseUniverse::makeCellAlive(row, col);
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::makeCellDead(size_t row, size_t col) {
    DenseUniverse::makeCellDead(row, col);
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::advance() {
    DenseUniverse::advance();
}

template <size_t Rows, size_t Cols>
std::vector<std::pair<size_t, size_t>> DenseUniverseV2<Rows, Cols>::getAliveCellsPos() const {
    return DenseUniverse::getAliveCellsPos();
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::save(const std::filesystem::path& file_path) const {
    DenseUniverse::save(file_path);
}

template <size_t Rows, size_t Cols>
void DenseUniverseV2<Rows, Cols>::load(const std::filesystem::path& file_path) {
    DenseUniverse::load(file_path);
}
#endif
