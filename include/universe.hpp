#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <filesystem>
#include <vector>
#include <memory>
#include <set>
#include <unordered_map>

#include "cell.hpp"

struct UniverseFileData;

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
        std::vector<std::pair<size_t, size_t>> getNeighborsPos(size_t row, size_t col);
        size_t m_rows;
        size_t m_cols;
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
        void createCells();
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


#endif
