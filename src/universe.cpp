#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <map>
#include <cmath>
#include <fstream>

#include "universe.hpp"
#include "cell.hpp"

struct UniverseFileData {
    size_t rows;
    size_t cols;
    std::vector<std::pair<size_t, size_t>> alive_cells_pos;
};

Universe::Universe(size_t rows, size_t cols): m_rows(rows), m_cols(cols) {
    size_t max_dim = static_cast<size_t>(pow(2, 32));
    if (rows > max_dim || cols > max_dim) {
        throw std::runtime_error("Universe cannot have rows/columns greater than 2^32");
    }
}

Universe::Universe(const std::filesystem::path& file_path) {}

void Universe::save(const std::filesystem::path& file_path) const {
    std::filesystem::path save_path(file_path);
    if (save_path.extension() != ".univ") {
        save_path = save_path.string() + ".univ";
    }
    std::ofstream file(save_path, std::ios::out);
    file << "GameOfLifeUniverse\n";
    file << m_rows << '\n';
    file << m_cols << '\n';
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open output file");
    }
    const auto& alive_cells = getAliveCellsPos();
    file << alive_cells.size() << '\n';
    for (const std::pair<size_t, size_t>& p: alive_cells) {
        file << p.first << ',' << p.second << '\n';
    }
    file.close();
}

UniverseFileData Universe::parseFile(const std::filesystem::path& file_path) {
    if (file_path.extension().string() != ".univ") {
        throw std::runtime_error(file_path.string() + " is not a .univ file");
    }
    std::ifstream file(file_path, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open universe file");
    }
    std::string header;
    file >> header;
    if (header != "GameOfLifeUniverse") {
        throw std::runtime_error("Not a valid universe file");
    }
    size_t rows, cols;
    file >> rows >> cols;
    size_t alive_count;
    file >> alive_count;
    std::vector<std::pair<size_t, size_t>> alive_cells_pos;
    for (size_t i = 0; i < alive_count; ++i) {
        std::string pos;
        file >> pos;
        auto n = pos.find(",");
        size_t row = std::stoi(pos.substr(0, n));
        size_t col = std::stoi(pos.substr(n + 1, pos.size() - n - 1));
        alive_cells_pos.push_back({row, col});
    }
    file.close();
    return {rows, cols, alive_cells_pos}; // TODO will this move or copy?
}

DenseUniverse::DenseUniverse(size_t rows, size_t cols): Universe(rows, cols) {
    createCells();
}

void DenseUniverse::createCells() {
    for (size_t row = 0; row < m_rows; ++row) {
        std::vector<std::unique_ptr<Cell>> cell_row;
        for (size_t col = 0; col < m_cols; ++col) {
            cell_row.push_back(std::make_unique<Cell>(row, col, m_cols * row + col, false));
        }
        m_cell_grid.push_back(std::move(cell_row));
    }
}

DenseUniverse::DenseUniverse(const std::filesystem::path& file_path): Universe(file_path) {
    auto fdata = Universe::parseFile(file_path);
    m_rows = fdata.rows;
    m_cols = fdata.cols;
    createCells();
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        m_cell_grid[p.first][p.second]->makeAlive();
    }
}

std::vector<Cell*> DenseUniverse::getNeighbors(Cell* cell) {
    std::vector<Cell*> neighbors;
    int64_t row_count = static_cast<int64_t>(m_rows);
    int64_t col_count = static_cast<int64_t>(m_cols);
    for (int dr = -1; dr < 2; dr++) {
        for (int dc = -1; dc < 2; dc++) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            int64_t nei_row = cell->row() + dr;
            int64_t nei_col = cell->col() + dc;
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

std::vector<std::pair<size_t, size_t>> DenseUniverse::getAliveCellsPos() const {
    std::vector<std::pair<size_t, size_t>> alive_pos;
    for (size_t row = 0; row < m_rows; row++) {
        for (size_t col = 0; col < m_cols; col++) {
            if (m_cell_grid[row][col]->isAlive()) {
                alive_pos.push_back({row, col});
            }
        }
    }
    return alive_pos;
}

void DenseUniverse::save(const std::filesystem::path& file_path) const {
    Universe::save(file_path);
}

void DenseUniverse::load(const std::filesystem::path& file_path) {
    for (size_t row = 0; row < m_rows; ++row) {
        for (size_t col = 0; col < m_cols; ++col) {
            m_cell_grid[row][col]->makeDead();
        }
    }
    auto fdata = Universe::parseFile(file_path);
    if (fdata.rows != m_rows || fdata.cols != m_cols) {
        throw std::runtime_error("Cannot load a universe with a mismatched size");
    }
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        m_cell_grid[p.first][p.second]->makeAlive();
    }
}

SparseUniverse::SparseUniverse(size_t rows, size_t cols): Universe(rows, cols) {}

SparseUniverse::SparseUniverse(const std::filesystem::path& file_path): Universe(file_path) {
    auto fdata = Universe::parseFile(file_path);
    m_rows = fdata.rows;
    m_cols = fdata.cols;
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        size_t row = p.first;
        size_t col = p.second;
        m_alive_cells.insert(std::make_unique<Cell>(row, col, m_cols * row + col, true));
    }
}

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
        int64_t row_count = static_cast<int64_t>(m_rows);
        int64_t col_count = static_cast<int64_t>(m_cols);
        size_t alive_count = 0;
        for (int dr = -1; dr < 2; dr++) {
            for (int dc = -1; dc < 2; dc++) {
                if (dr == 0 && dc == 0) {
                    continue;
                }
                int64_t nei_row = cell->row() + dr;
                int64_t nei_col = cell->col() + dc;
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

std::vector<std::pair<size_t, size_t>> SparseUniverse::getAliveCellsPos() const {
    std::vector<std::pair<size_t, size_t>> alive_pos;
    for (const std::unique_ptr<Cell>& cell: m_alive_cells) {
        alive_pos.push_back({cell->row(), cell->col()});
    }
    return alive_pos;
}

void SparseUniverse::save(const std::filesystem::path& file_path) const {
    Universe::save(file_path);
}

void SparseUniverse::load(const std::filesystem::path& file_path) {
    m_alive_cells.clear();
    auto fdata = Universe::parseFile(file_path);
    if (fdata.rows != m_rows || fdata.cols != m_cols) {
        throw std::runtime_error("Cannot load a universe with a mismatched size");
    }
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        size_t row = p.first;
        size_t col = p.second;
        m_alive_cells.insert(std::make_unique<Cell>(row, col, m_cols * row + col, true));
    }
}
