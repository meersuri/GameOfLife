#include <thread>

#include "animator.hpp"
#include "painter.hpp"
#include "universe.hpp"

Animator::Animator(std::chrono::milliseconds refresh_period):
    m_refresh_period(refresh_period), m_painter(GridPainter()) {}

void Animator::printRowOffset(size_t offset, Color color) {
    std::string row_offset_str = std::to_string(offset);
    for (size_t i = 0; i < row_offset_str.size(); ++i) {
        m_painter.paint(i + 1, 0, static_cast<char>(row_offset_str[i]), color);
    }
}

void Animator::printColOffset(size_t offset, Color color) {
    std::string col_offset = std::to_string(offset);
    for (size_t i = 0; i < col_offset.size(); ++i) {
        m_painter.paint(0, i + 1, static_cast<char>(col_offset[i]), color);
    }
}

void Animator::paintMargins(size_t row_count, size_t col_count, size_t thickness, Color color) {
    for (size_t row = 1; row < row_count + 1; ++row) {
        for (size_t col = 0; col < thickness; ++col) {
            m_painter.paint(row, col, "█", color);
        }
    }
    for (size_t col = 1; col < col_count + 1; ++col) {
        for (size_t row = 0; row < thickness; ++row) {
            m_painter.paint(row, col, "█", color);
        }
    }
}

FullViewAnimator::FullViewAnimator(std::chrono::milliseconds refresh_period): Animator(refresh_period) {}

void FullViewAnimator::animate(Universe* universe, size_t time_steps) {
    size_t max_row = 0;
    size_t max_col = 0;
    size_t margin_thickness = 1;

    m_painter.clear();
    for (size_t i = 0; i < time_steps; ++i) {
        max_row = 0;
        max_col = 0;
        for (const auto& [row, col]: universe->getAliveCellsPos()) {
            size_t cell_row = row + margin_thickness;
            size_t cell_col = col + margin_thickness;
            m_painter.paint(cell_row, cell_col, "█", Color::green); // add margins
            max_row = std::max(cell_row, max_row);
            max_col = std::max(cell_col, max_col);
        }
        // row, col offset is always zero
        printRowOffset(0);
        printColOffset(0);
        universe->advance();
        std::this_thread::sleep_for(m_refresh_period);
        m_painter.shiftCursor(max_row, max_col); // since clear is from current cursor pos up, track max row col
        m_painter.clear();
    }
    m_painter.shiftCursor(max_row + 1, 1);
}

AutoPanAnimator::AutoPanAnimator(std::chrono::milliseconds refresh_period): Animator(refresh_period) {}

// simplest auto-pan is to create a bounding box around alive cells and translate to top left
void AutoPanAnimator::animate(Universe* universe, size_t time_steps) {
    size_t max_row = 0;
    size_t max_col = 0;
    size_t margin_thickness = 1;
    int64_t min_row = universe->rowCount();
    int64_t min_col = universe->colCount();

    m_painter.clear();
    for (size_t i = 0; i < time_steps; ++i) {
        max_row = 0;
        max_col = 0;
        min_row = universe->rowCount();
        min_col = universe->colCount();
        for (const auto& [row, col]: universe->getAliveCellsPos()) {
            min_row = std::min(static_cast<int64_t>(row), min_row); // safe cast: max pos 2**32
            max_row = std::max(row, max_row);
            min_col = std::min(static_cast<int64_t>(col), min_col); // safe cast: max pos 2**32
            max_col = std::max(col, max_col);
        }
        for (const auto& [row, col]: universe->getAliveCellsPos()) {
            size_t cell_row = row - min_row + margin_thickness;
            size_t cell_col = col - min_col + margin_thickness;
            m_painter.paint(cell_row, cell_col, "█", Color::green); // translate to top left with a margin
        }
        printRowOffset(min_row);
        printColOffset(min_col);
        universe->advance();
        std::this_thread::sleep_for(m_refresh_period);
        m_painter.shiftCursor(max_row - min_row + margin_thickness, max_col - min_col + margin_thickness);
        m_painter.clear();
    }
    m_painter.shiftCursor(max_row - min_row + margin_thickness, 1);
}
