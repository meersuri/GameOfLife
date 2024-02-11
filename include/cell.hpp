#ifndef CELL_HPP
#define CELL_HPP

#include <vector>
#include <cstddef>

class Cell {
    public:
        Cell(size_t row, size_t col, size_t flat_pos, bool alive=false);
        bool isAlive() const;
        size_t row() const { return m_row; }
        size_t col() const { return m_col; }
        size_t flatPos() const { return m_flat_pos; }
        void makeAlive();
        void makeDead();
    private:
        size_t m_row; // tracks its position within the Universe
        size_t m_col;
        size_t m_flat_pos;
        bool m_is_alive{false};
};

bool operator<(const std::unique_ptr<Cell>& a, const std::unique_ptr<Cell>& b);

#endif
