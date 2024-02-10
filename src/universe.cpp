#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <map>

#include "universe.hpp"
#include "cell.hpp"

Universe::Universe(size_t rows, size_t cols): m_rows(rows), m_cols(cols) {}

DenseUniverse::DenseUniverse(size_t rows, size_t cols): Universe(rows, cols) {
    for (size_t row = 0; row < rows; ++row) {
        std::vector<std::unique_ptr<Cell>> cell_row;
        for (size_t col = 0; col < cols; ++col) {
            cell_row.push_back(std::make_unique<Cell>(row, col, m_cols * row + col, false));
        }
        m_cell_grid.push_back(std::move(cell_row));
    }
}

std::vector<Cell*> DenseUniverse::getNeighbors(Cell* cell) {
    std::vector<Cell*> neighbors;
    int row_count = static_cast<int>(m_rows);
    int col_count = static_cast<int>(m_cols);
    for (int dr = -1; dr < 2; dr++) {
        for (int dc = -1; dc < 2; dc++) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            int nei_row = cell->row() + dr;
            int nei_col = cell->col() + dc;
            if (nei_row < 0 || nei_row >= row_count || nei_col < 0 || nei_col >= col_count) {
                continue;
            }
            neighbors.push_back(m_cell_grid[nei_row][nei_col].get());
        }
    }
    return neighbors;
}

bool DenseUniverse::isCellAlive(size_t row, size_t col) {
    // TODO adds bounds checking?
    return m_cell_grid[row][col]->isAlive();
}

void DenseUniverse::makeCellAlive(size_t row, size_t col) {
    m_cell_grid[row][col]->makeAlive();
}

void DenseUniverse::makeCellDead(size_t row, size_t col) {
    m_cell_grid[row][col]->makeDead();
}

void DenseUniverse::advance() {
    // copy the whole grid - TODO can we do better?
    std::vector<std::vector<std::unique_ptr<Cell>>> grid_copy;
    for (size_t row = 0; row < m_rows; row++) {
        const auto& cell_row = m_cell_grid[row];
        std::vector<std::unique_ptr<Cell>> row_copy;
        for (size_t col = 0; col < m_cols; col++) {
            Cell* cell = cell_row[col].get();
            row_copy.push_back(std::make_unique<Cell>(row, col, m_cols * row + col, cell->isAlive()));
        }
        grid_copy.push_back(std::move(row_copy));
    }
    // update each cell in the copy
    for (size_t row = 0; row < m_rows; row++) {
        for (size_t col = 0; col < m_cols; col++) {
            Cell* cell = m_cell_grid[row][col].get();
            size_t alive_count = 0;
            for (Cell* neighbor: getNeighbors(cell)) {
                alive_count = neighbor->isAlive() ? alive_count + 1: alive_count;
            }
            if (cell->isAlive()) {
                if (alive_count < 2 || alive_count > 3) {
                    grid_copy[row][col]->makeDead();
                }
            }
            else if (alive_count == 3) {
                grid_copy[row][col]->makeAlive();
            }
        }
    }
    std::swap(m_cell_grid, grid_copy);
}

std::vector<std::pair<int, int>> DenseUniverse::getAliveCellsPos() {
    std::vector<std::pair<int, int>> alive_pos;
    for (size_t row = 0; row < m_rows; row++) {
        for (size_t col = 0; col < m_cols; col++) {
            if (m_cell_grid[row][col]->isAlive()) {
                alive_pos.push_back({row, col});
            }
        }
    }
    return alive_pos;
}


SparseUniverse::SparseUniverse(size_t rows, size_t cols): Universe(rows, cols) {}

std::set<std::unique_ptr<Cell>>::iterator SparseUniverse::findAliveCellByPos(size_t row, size_t col) {
    size_t target = m_cols * row + col;
    auto iter = std::lower_bound(m_alive_cells.begin(), m_alive_cells.end(), target,
            [](const std::unique_ptr<Cell>& a, size_t flat_pos) {
                return  a->flatPos() < flat_pos;
            });
    if (iter == m_alive_cells.end() || (*iter)->flatPos() > target) {
        return m_alive_cells.end();
    }
    return iter;
}

bool SparseUniverse::isCellAlive(size_t row, size_t col) {
    return findAliveCellByPos(row, col) != m_alive_cells.end();
}

void SparseUniverse::makeCellAlive(size_t row, size_t col) {
    auto iter = findAliveCellByPos(row, col);
    if (iter == m_alive_cells.end()) {
        auto cell = std::make_unique<Cell>(row, col, m_cols * row + col, true);
        m_alive_cells.insert(std::move(cell));
    }
}

void SparseUniverse::makeCellDead(size_t row, size_t col) {
    auto iter = findAliveCellByPos(row, col);
    if (iter != m_alive_cells.end()) {
        m_alive_cells.erase(iter);
    }
}

void SparseUniverse::advance() {
    // frontier: cells that are 8-connected adjacent to alive cells
    // only the frontier cells can come alive in the next generation
    // track how many alive neighbors each frontier cell has
    std::map<std::pair<int, int>, size_t> frontier_hit_count;
    std::set<std::unique_ptr<Cell>> next_alive_cells;
    for (const std::unique_ptr<Cell>& cell: m_alive_cells) {
        int row_count = static_cast<int>(m_rows);
        int col_count = static_cast<int>(m_cols);
        size_t alive_count = 0;
        for (int dr = -1; dr < 2; dr++) {
            for (int dc = -1; dc < 2; dc++) {
                if (dr == 0 && dc == 0) {
                    continue;
                }
                int nei_row = cell->row() + dr;
                int nei_col = cell->col() + dc;
                if (nei_row < 0 || nei_row >= row_count || nei_col < 0 || nei_col >= col_count) {
                    continue;
                }
                if (findAliveCellByPos(nei_row, nei_col) == m_alive_cells.end()) {
                    frontier_hit_count[{nei_row, nei_col}]++;
                }
                else {
                    alive_count++;
                }
            }
        }
        if (alive_count == 2 || alive_count == 3) {
            auto cell_copy = std::make_unique<Cell>(cell->row(), cell->col(), cell->flatPos(), true);
            next_alive_cells.insert(std::move(cell_copy));
        }
    }
    for (auto& p: frontier_hit_count) {
        if (p.second != 3) {
            continue;
        }
        size_t row = p.first.first;
        size_t col = p.first.second;
        auto new_cell = std::make_unique<Cell>(p.first.first, p.first.second, m_cols * row + col, true);
        next_alive_cells.insert(std::move(new_cell));
    }
    std::swap(m_alive_cells, next_alive_cells);
}

std::vector<std::pair<int, int>> SparseUniverse::getAliveCellsPos() {
    std::vector<std::pair<int, int>> alive_pos;
    for (const std::unique_ptr<Cell>& cell: m_alive_cells) {
        alive_pos.push_back({cell->row(), cell->col()});
    }
    return alive_pos;
}
