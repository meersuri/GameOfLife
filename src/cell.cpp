#include <memory>

#include "cell.hpp"

Cell::Cell(size_t row, size_t col, size_t flat_pos, bool alive):
    m_row(row), m_col(col), m_flat_pos(flat_pos), m_is_alive(alive) {}

bool Cell::isAlive() const {
    return m_is_alive;
}

void Cell::makeAlive() {
    m_is_alive = true;
}

void Cell::makeDead() {
    m_is_alive = false;
}
