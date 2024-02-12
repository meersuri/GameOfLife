#include <iostream>

#include "painter.hpp"

GridPainter::GridPainter() {
    std::cout << m_esc << "?25l" << std::flush; // hide cursor
}

GridPainter::~GridPainter() {
    std::cout << m_esc << "?25h" << std::flush; // show cursor
}

void GridPainter::clear() {
    std::cout << m_esc << "1J" << std::flush; // clear from cursor to top
}

void GridPainter::shiftCursor(size_t row, size_t col) { // one based line, col (1, 1) is top left
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
