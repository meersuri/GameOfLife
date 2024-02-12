#include <thread>

#include "animator.hpp"
#include "painter.hpp"
#include "universe.hpp"

Animator::Animator(std::chrono::milliseconds refresh_period):
    m_refresh_period(refresh_period), m_painter(GridPainter()) {}

void Animator::printRowOffset(size_t offset, Color color) {
    std::string row_offset_str = std::to_string(offset);
    for (size_t i = 0; i < row_offset_str.size(); ++i) {
        m_painter.paint(i + 2, 1, static_cast<char>(row_offset_str[i]), color);
    }
}

void Animator::printColOffset(size_t offset, Color color) {
    std::string col_offset = std::to_string(offset);
    for (size_t i = 0; i < col_offset.size(); ++i) {
        m_painter.paint(1, i + 2, static_cast<char>(col_offset[i]), color);
    }
}

FullViewAnimator::FullViewAnimator(std::chrono::milliseconds refresh_period): Animator(refresh_period) {}

void FullViewAnimator::animate(Universe* universe, size_t time_steps) {
    m_painter.clear();
    size_t max_row = 0;
    size_t max_col = 0;
    for (size_t i = 0; i < time_steps; ++i) {
        max_row = 0;
        max_col = 0;
        for (const std::pair<size_t, size_t>& pos: universe->getAliveCellsPos()) {
            m_painter.paint(pos.first + 2, pos.second + 2, '*', Color::green); // add margins
            max_row = std::max(pos.first + 2, max_row);
            max_col = std::max(pos.second + 2, max_col);
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
    m_painter.clear();
    size_t max_row = 0;
    size_t max_col = 0;
    int64_t min_row = universe->rowCount();
    int64_t min_col = universe->colCount();
    for (size_t i = 0; i < time_steps; ++i) {
        max_row = 0;
        max_col = 0;
        min_row = universe->rowCount();
        min_col = universe->colCount();
        for (const std::pair<size_t, size_t>& pos: universe->getAliveCellsPos()) {
            min_row = std::min(static_cast<int64_t>(pos.first), min_row); // safe cast: max pos 2**32
            max_row = std::max(pos.first, max_row);
            min_col = std::min(static_cast<int64_t>(pos.second), min_col); // safe cast: max pos 2**32
            max_col = std::max(pos.second, max_col);
        }
        for (const std::pair<size_t, size_t>& pos: universe->getAliveCellsPos()) {
            m_painter.paint(pos.first - min_row + 2, pos.second - min_col + 2, '*', Color::green); // translate to top left with a margin
        }
        printRowOffset(min_row);
        printColOffset(min_col);
        universe->advance();
        std::this_thread::sleep_for(m_refresh_period);
        m_painter.shiftCursor(max_row - min_row + 2, max_col - min_col + 2); // (min_row, min_col) translated to (2, 2)
        m_painter.clear();
    }
    m_painter.shiftCursor(max_row - min_row + 1, 1);
}
