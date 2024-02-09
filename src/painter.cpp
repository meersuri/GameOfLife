#include <iostream>

#include "painter.hpp"

GridPainter::GridPainter(size_t rows, size_t cols): m_rows(rows), m_cols(cols) {
    std::cout << m_esc << "?25l" << std::flush;
}

GridPainter::~GridPainter() {
    std::cout << m_esc << "?25h" << std::flush;
}

void GridPainter::clear() {
    std::cout << m_esc << "1J" << std::flush;
}

void GridPainter::shiftCursor(size_t row, size_t col) {
    std::cout << m_esc << std::to_string(row) << ";" << std::to_string(col) << "H" << std::flush;
}

void GridPainter::paint(char cell_char, Color color) {
    std::string code = m_esc + std::to_string(static_cast<int>(color)) + "m";
    std::cout << code << cell_char << m_reset_style << std::flush;
}

void GridPainter::paint(size_t row, size_t col, char cell_char, Color color) {
    shiftCursor(row, col);
    paint(cell_char, color);
}
