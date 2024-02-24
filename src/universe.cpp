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

std::vector<std::pair<size_t, size_t>> Universe::getNeighborsPos(size_t row, size_t col) {
    int64_t row_count = static_cast<int64_t>(m_rows);
    int64_t col_count = static_cast<int64_t>(m_cols);
    std::vector<std::pair<size_t, size_t>> pos;
    for (int dr = -1; dr < 2; dr++) {
        for (int dc = -1; dc < 2; dc++) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            int64_t nei_row = row + dr;
            int64_t nei_col = col + dc;
            if (nei_row < 0 || nei_row >= row_count || nei_col < 0 || nei_col >= col_count) {
                continue;
            }
            pos.push_back({nei_row, nei_col});
        }
    }
    return pos;
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

std::vector<std::vector<Cell>>& DenseUniverse::getCurrentGrid() {
    return m_grid_1_is_current ? m_cell_grid_1 : m_cell_grid_2;
}

const std::vector<std::vector<Cell>>& DenseUniverse::getCurrentGrid() const {
    return m_grid_1_is_current ? m_cell_grid_1 : m_cell_grid_2;
}

void DenseUniverse::createCells() {
    for (size_t row = 0; row < m_rows; ++row) {
        std::vector<Cell> cell_row_1;
        std::vector<Cell> cell_row_2;
        for (size_t col = 0; col < m_cols; ++col) {
            cell_row_1.emplace_back(row, col, m_cols * row + col, false);
            cell_row_2.emplace_back(row, col, m_cols * row + col, false);
        }
        m_cell_grid_1.push_back(std::move(cell_row_1));
        m_cell_grid_2.push_back(std::move(cell_row_2));
    }
}

DenseUniverse::DenseUniverse(const std::filesystem::path& file_path): Universe(file_path) {
    auto fdata = Universe::parseFile(file_path);
    m_rows = fdata.rows;
    m_cols = fdata.cols;
    createCells();
    auto& grid = getCurrentGrid();
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        grid[p.first][p.second].makeAlive();
    }
}

std::vector<Cell*> DenseUniverse::getNeighbors(const Cell& cell) {
    std::vector<Cell*> neighbors;
    auto& grid = getCurrentGrid();
    for (const auto&[row, col]: getNeighborsPos(cell.row(), cell.col())) {
        neighbors.push_back(&grid[row][col]);
    }
    return neighbors;
}

bool DenseUniverse::isCellAlive(size_t row, size_t col) {
    // TODO adds bounds checking?
    const auto& grid = getCurrentGrid();
    return grid[row][col].isAlive();
}

void DenseUniverse::makeCellAlive(size_t row, size_t col) {
    auto& grid = getCurrentGrid();
    grid[row][col].makeAlive();
}

void DenseUniverse::makeCellDead(size_t row, size_t col) {
    auto& grid = getCurrentGrid();
    grid[row][col].makeDead();
}

void DenseUniverse::advance() {
    const auto& grid = getCurrentGrid();
    auto& next_grid = m_grid_1_is_current ? m_cell_grid_2 : m_cell_grid_1;

    for (size_t row = 0; row < m_rows; row++) {
        for (size_t col = 0; col < m_cols; col++) {
            const Cell& cell = grid[row][col];
            size_t alive_count = 0;
            for (Cell* neighbor: getNeighbors(cell)) {
                alive_count = neighbor->isAlive() ? alive_count + 1: alive_count;
            }
            if (cell.isAlive()) {
                if (alive_count < 2 || alive_count > 3) {
                    next_grid[row][col].makeDead();
                }
                else {
                    next_grid[row][col].makeAlive();
                }
            }
            else {
                if (alive_count == 3) {
                    next_grid[row][col].makeAlive();
                }
                else {
                    next_grid[row][col].makeDead();
                }
            }
        }
    }

    m_grid_1_is_current = !m_grid_1_is_current;
}

std::vector<std::pair<size_t, size_t>> DenseUniverse::getAliveCellsPos() const {
    std::vector<std::pair<size_t, size_t>> alive_pos;
    const auto& grid = getCurrentGrid();
    for (size_t row = 0; row < m_rows; row++) {
        for (size_t col = 0; col < m_cols; col++) {
            if (grid[row][col].isAlive()) {
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
    auto& grid = getCurrentGrid();
    for (size_t row = 0; row < m_rows; ++row) {
        for (size_t col = 0; col < m_cols; ++col) {
            grid[row][col].makeDead();
        }
    }
    auto fdata = Universe::parseFile(file_path);
    if (fdata.rows != m_rows || fdata.cols != m_cols) {
        throw std::runtime_error("Cannot load a universe with a mismatched size");
    }
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        grid[p.first][p.second].makeAlive();
    }
}


SparseUniverse::SparseUniverse(size_t rows, size_t cols): Universe(rows, cols) {}

SparseUniverse::SparseUniverse(const std::filesystem::path& file_path): Universe(file_path) {}

bool SparseUniverse::isCellAlive(size_t row, size_t col) {
    return findAliveCellByPos(row, col) != nullptr;
}

void SparseUniverse::makeCellAlive(size_t row, size_t col) {
    if (!findAliveCellByPos(row, col)) {
        makeAndInsertAliveCell(row, col);
    }
}

void SparseUniverse::makeCellDead(size_t row, size_t col) {
    deleteCell(row, col);
}

void SparseUniverse::advance() {
    // frontier: cells that are 8-connected adjacent to alive cells
    // only the frontier cells can come alive in the next generation
    // track how many alive neighbors each frontier cell has
    clearNextBuffer();
    std::unordered_map<size_t, size_t> frontier_hit_count;
    for (Cell* cell: getAliveCells()) {
        size_t alive_count = 0;
        for (const auto& [nei_row, nei_col]: getNeighborsPos(cell->row(), cell->col())) {
            if (!findAliveCellByPos(nei_row, nei_col)) {
                frontier_hit_count[{m_cols * nei_row + nei_col}]++;
            }
            else {
                alive_count++;
            }
        }
        if (alive_count == 2 || alive_count == 3) {
            makeAndInsertNextAliveCell(cell->row(), cell->col());
        }
    }

    for (const auto& [flat_pos, alive_count]: frontier_hit_count) {
        if (alive_count != 3) {
            continue;
        }
        size_t row = flat_pos / m_cols;
        size_t col = flat_pos % m_cols;
        makeAndInsertNextAliveCell(row, col);
    }
    swapBuffers();
}

void SparseUniverse::save(const std::filesystem::path& file_path) const {
    Universe::save(file_path);
}

void SparseUniverse::load(const std::filesystem::path& file_path) {
    clearBuffer();
    auto fdata = Universe::parseFile(file_path);
    if (fdata.rows != m_rows || fdata.cols != m_cols) {
        throw std::runtime_error("Cannot load a universe with a mismatched size");
    }
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        makeAndInsertAliveCell(p.first, p.second);
    }
}

SparseUniverseV1::SparseUniverseV1(size_t rows, size_t cols): SparseUniverse(rows, cols) {}

SparseUniverseV1::SparseUniverseV1(const std::filesystem::path& file_path): SparseUniverse(file_path) {
    auto fdata = Universe::parseFile(file_path);
    m_rows = fdata.rows;
    m_cols = fdata.cols;
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        size_t row = p.first;
        size_t col = p.second;
        makeAndInsertAliveCell(row, col);
    }
}

bool SparseUniverseV1::isCellAlive(size_t row, size_t col) {
    return SparseUniverse::isCellAlive(row, col);
}

void SparseUniverseV1::makeCellAlive(size_t row, size_t col) {
    return SparseUniverse::makeCellAlive(row, col);
}

void SparseUniverseV1::makeCellDead(size_t row, size_t col) {
    SparseUniverse::makeCellDead(row, col);
}

std::set<std::unique_ptr<Cell>>::iterator SparseUniverseV1::findAliveCellIterByPos(size_t row, size_t col) {
    size_t target = m_cols * row + col;
    auto it = std::lower_bound(m_alive_cells.begin(), m_alive_cells.end(), target,
            [](const std::unique_ptr<Cell>& a, size_t flat_pos) {
                return  a->flatPos() < flat_pos;
            });
    return it;
}

Cell* SparseUniverseV1::findAliveCellByPos(size_t row, size_t col) {
    auto it = findAliveCellIterByPos(row, col);
    size_t target = m_cols * row + col;
    if (it == m_alive_cells.end() || (*it)->flatPos() > target) {
        return nullptr;
    }
    return it->get();
}

std::vector<std::pair<size_t, size_t>> SparseUniverseV1::getAliveCellsPos() const {
    std::vector<std::pair<size_t, size_t>> alive_pos;
    for (const std::unique_ptr<Cell>& cell: m_alive_cells) {
        alive_pos.push_back({cell->row(), cell->col()});
    }
    return alive_pos;
}

std::vector<Cell*> SparseUniverseV1::getAliveCells() {
    std::vector<Cell*> cells;
    for (const std::unique_ptr<Cell>& p: m_alive_cells) {
        cells.push_back(p.get());
    }
    return cells;
}

void SparseUniverseV1::makeAndInsertAliveCell(size_t row, size_t col) {
    auto cell = std::make_unique<Cell>(row, col, m_cols * row + col, true);
    m_alive_cells.insert(std::move(cell));
}

void SparseUniverseV1::makeAndInsertNextAliveCell(size_t row, size_t col) {
    auto cell = std::make_unique<Cell>(row, col, m_cols * row + col, true);
    m_next_alive_cells.insert(std::move(cell));
}

void SparseUniverseV1::deleteCell(size_t row, size_t col) {
    auto it = findAliveCellIterByPos(row, col);
    if (it != m_alive_cells.end()) {
        m_alive_cells.erase(it);
    }
}

void SparseUniverseV1::advance() {
    SparseUniverse::advance();
}

void SparseUniverseV1::swapBuffers() {
    std::swap(m_alive_cells, m_next_alive_cells);
}

void SparseUniverseV1::clearBuffer() {
    m_alive_cells.clear();
}

void SparseUniverseV1::clearNextBuffer() {
    m_next_alive_cells.clear();
}

void SparseUniverseV1::save(const std::filesystem::path& file_path) const {
    SparseUniverse::save(file_path);
}

void SparseUniverseV1::load(const std::filesystem::path& file_path) {
    SparseUniverse::load(file_path);
}

SparseUniverseV2::SparseUniverseV2(size_t rows, size_t cols): SparseUniverse(rows, cols) {}

SparseUniverseV2::SparseUniverseV2(const std::filesystem::path& file_path): SparseUniverse(file_path) {
    auto fdata = Universe::parseFile(file_path);
    m_rows = fdata.rows;
    m_cols = fdata.cols;
    for (const std::pair<size_t, size_t>& p: fdata.alive_cells_pos) {
        makeAndInsertAliveCell(p.first, p.second);
    }
}

bool SparseUniverseV2::isCellAlive(size_t row, size_t col) {
    return SparseUniverse::isCellAlive(row, col);
}

void SparseUniverseV2::makeCellAlive(size_t row, size_t col) {
    return SparseUniverse::makeCellAlive(row, col);
}

void SparseUniverseV2::makeCellDead(size_t row, size_t col) {
    SparseUniverse::makeCellDead(row, col);
}

Cell* SparseUniverseV2::findAliveCellByPos(size_t row, size_t col) {
    size_t flat_pos = m_cols * row + col;
    auto it = m_alive_cells.find(flat_pos);
    if (it == m_alive_cells.end()) {
        return nullptr;
    }
    return &it->second;
}

void SparseUniverseV2::advance() {
    SparseUniverse::advance();
}

std::vector<Cell*> SparseUniverseV2::getAliveCells() {
    std::vector<Cell*> cells;
    for (auto it = m_alive_cells.begin(); it != m_alive_cells.end(); ++it) {
        cells.push_back(&(it->second));
    }
    return cells;
}

void SparseUniverseV2::makeAndInsertAliveCell(size_t row, size_t col) {
    size_t flat_pos = m_cols * row + col;
    m_alive_cells.emplace(flat_pos, Cell(row, col, flat_pos, true));
}

void SparseUniverseV2::makeAndInsertNextAliveCell(size_t row, size_t col) {
    size_t flat_pos = m_cols * row + col;
    m_next_alive_cells.emplace(flat_pos, Cell(row, col, flat_pos, true));
}

void SparseUniverseV2::deleteCell(size_t row, size_t col) {
    auto it = m_alive_cells.find(row * m_cols + col);
    if (it != m_alive_cells.end()) {
        m_alive_cells.erase(it);
    }
}

void SparseUniverseV2::swapBuffers() {
    std::swap(m_alive_cells, m_next_alive_cells);
}

void SparseUniverseV2::clearBuffer() {
    m_alive_cells.clear();
}

void SparseUniverseV2::clearNextBuffer() {
    m_next_alive_cells.clear();
}

std::vector<std::pair<size_t, size_t>> SparseUniverseV2::getAliveCellsPos() const {
    std::vector<std::pair<size_t, size_t>> alive_pos;
    for (const auto& [flat_pos, cell]: m_alive_cells) {
        alive_pos.push_back({cell.row(), cell.col()});
    }
    return alive_pos;
}

void SparseUniverseV2::save(const std::filesystem::path& file_path) const {
    SparseUniverse::save(file_path);
}

void SparseUniverseV2::load(const std::filesystem::path& file_path) {
    SparseUniverse::load(file_path);
}
