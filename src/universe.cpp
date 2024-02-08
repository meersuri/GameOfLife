#include <vector>
#include <memory>
#include <iostream>

#include "universe.hpp"
#include "cell.hpp"

Universe::Universe(size_t rows, size_t cols): m_rows(rows), m_cols(cols) {}

DenseUniverse::DenseUniverse(size_t rows, size_t cols): Universe(rows, cols) {
    for (size_t row = 0; row < rows; ++row) {
        std::vector<std::unique_ptr<Cell>> cell_row;
        for (size_t col = 0; col < cols; ++col) {
            cell_row.push_back(std::make_unique<Cell>(false));
        }
        m_cell_grid.push_back(std::move(cell_row));
    }
    computeNeighbors();
}

void DenseUniverse::computeNeighbors() {
    int row_count = static_cast<int>(m_rows);
    int col_count = static_cast<int>(m_cols);
    for (int row = 0; row < row_count; ++row) {
        for (int col = 0; col < col_count; ++col) {
            Cell* cell = m_cell_grid[row][col].get();
            for (int dr = -1; dr < 2; dr++) {
                for (int dc = -1; dc < 2; dc++) {
                    if (dr == 0 && dc == 0) {
                        continue;
                    }
                    int nei_row = (row) + dr;
                    int nei_col = (col) + dc;
                    if (nei_row < 0 || nei_row >= row_count || nei_col < 0 || nei_col >= col_count) {
                        continue;
                    }
                    cell->m_neighbors.push_back(m_cell_grid[nei_row][nei_col].get());
                }
            }
        }
    }
}

Cell* DenseUniverse::cell(size_t row, size_t col) {
    // TODO adds bounds checking?
    return m_cell_grid[row][col].get();
}

void DenseUniverse::advance() {
    std::vector<std::vector<std::unique_ptr<Cell>>> grid_copy;
    for (const std::vector<std::unique_ptr<Cell>>& row: m_cell_grid) {
        std::vector<std::unique_ptr<Cell>> row_copy;
        for (const std::unique_ptr<Cell>& cell: row) {
            row_copy.push_back(std::make_unique<Cell>(cell->isAlive()));
        }
        grid_copy.push_back(std::move(row_copy));
    }

    for (size_t row = 0; row < m_rows; row++) {
        for (size_t col = 0; col < m_cols; col++) {
            Cell* cell = m_cell_grid[row][col].get();
            size_t alive_count = 0;
            for (Cell* neighbor: cell->neighbors()) {
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
    computeNeighbors();
}

