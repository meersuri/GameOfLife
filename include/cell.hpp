#ifndef CELL_HPP
#define CELL_HPP

#include <vector>

class DenseUniverse; //fwd declare

class Cell {
    friend class DenseUniverse;
    public:
        Cell(bool alive=false);
        bool isAlive() const;
        void makeAlive();
        void makeDead();
        const std::vector<Cell*> neighbors() const;
    private:
        bool m_is_alive{false};
        std::vector<Cell*> m_neighbors;
};

#endif
