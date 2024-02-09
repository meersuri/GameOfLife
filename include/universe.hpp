#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <filesystem>
#include <vector>
#include <memory>

class Cell; //forward declare;

// defines the interface for a Universe of Cells
class Universe {
    public:
        Universe(size_t rows, size_t cols);
        virtual void advance() = 0;
        virtual bool isCellAlive(size_t row, size_t col) = 0;
        virtual void makeCellAlive(size_t row, size_t col) = 0;
        virtual void makeCellDead(size_t row, size_t col) = 0;
        size_t rowCount() const { return m_rows; }
        size_t colCount() const { return m_cols; }
        virtual ~Universe() {};
    protected:
        size_t m_rows;
        size_t m_cols;
};

// keeps all Cells in memory
class DenseUniverse: public Universe {
    public:
        DenseUniverse(size_t rows, size_t cols);
        void advance() override;
        bool isCellAlive(size_t row, size_t col) override;
        void makeCellAlive(size_t row, size_t col) override;
        void makeCellDead(size_t row, size_t col) override;
    private:
        std::vector<Cell*> getNeighbors(Cell* cell);
        std::vector<std::vector<std::unique_ptr<Cell>>> m_cell_grid;
};

#endif
