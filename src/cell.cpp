#include "cell.hpp"

Cell::Cell(bool alive): m_is_alive(alive) {}

bool Cell::isAlive() const {
    return m_is_alive;
}

void Cell::makeAlive() {
    m_is_alive = true;
}

void Cell::makeDead() {
    m_is_alive = false;
}

const std::vector<Cell*> Cell::neighbors() const {
    return m_neighbors;
}

