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
        std::vector<Cell*> getNeighbors(Cell* cell);
        std::vector<std::vector<std::unique_ptr<Cell>>> m_cell_grid;
};

// keeps only alive Cells in memory
class SparseUniverse: public Universe {
    public:
        SparseUniverse(size_t rows, size_t cols);
        SparseUniverse(const std::filesystem::path& file_path);
        void advance() override;
        bool isCellAlive(size_t row, size_t col) override;
        void makeCellAlive(size_t row, size_t col) override;
        void makeCellDead(size_t row, size_t col) override;
        std::vector<std::pair<size_t, size_t>> getAliveCellsPos() const override;
        void save(const std::filesystem::path& file_path) const override;
        void load(const std::filesystem::path& file_path) override;
    private:
        std::set<std::unique_ptr<Cell>>::iterator findAliveCellByPos(size_t row, size_t col);
        std::set<std::unique_ptr<Cell>> m_alive_cells;
};

class SparseUniverseV2: public Universe {
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
        std::unordered_map<size_t, Cell>::iterator findAliveCellByPos(size_t row, size_t col);
        std::unordered_map<size_t, Cell> m_alive_cells;
};


#endif
