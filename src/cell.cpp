#include <memory>

#include "cell.hpp"

Cell::Cell():
    m_row(0), m_col(0), m_flat_pos(0), m_is_alive(false) {}

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

bool operator<(const std::unique_ptr<Cell>& a, const std::unique_ptr<Cell>& b) {
    return a->flatPos() < b->flatPos();
}

