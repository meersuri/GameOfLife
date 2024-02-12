#include <thread>

#include "animator.hpp"
#include "painter.hpp"
#include "universe.hpp"

Animator::Animator(std::chrono::milliseconds refresh_period): m_refresh_period(refresh_period) {}

FullViewAnimator::FullViewAnimator(std::chrono::milliseconds refresh_period): Animator(refresh_period) {}

void FullViewAnimator::animate(Universe* universe, size_t time_steps) {
    GridPainter painter(universe->rowCount(), universe->colCount());
    painter.clear();
    size_t max_row = 0;
    size_t max_col = 0;
    for (size_t i = 0; i < time_steps; ++i) {
        max_row = 0;
        max_col = 0;
        for (const std::pair<size_t, size_t>& pos: universe->getAliveCellsPos()) {
            painter.paint(pos.first + 2, pos.second + 2, '*', Color::green); // add margins
            max_row = std::max(pos.first + 2, max_row);
            max_col = std::max(pos.second + 2, max_col);
        }
        painter.paint(2, 1, '0', Color::yellow);
        painter.paint(1, 2, '0', Color::yellow);
        universe->advance();
        std::this_thread::sleep_for(m_refresh_period);
        painter.shiftCursor(max_row, max_col); // since clear is from current cursor pos up, track max row col
        painter.clear();
    }
    painter.shiftCursor(max_row + 1, 1);
}

AutoPanAnimator::AutoPanAnimator(std::chrono::milliseconds refresh_period): Animator(refresh_period) {}

// simplest auto-pan is to create a bounding box around alive cells and translate to top left
void AutoPanAnimator::animate(Universe* universe, size_t time_steps) {
    GridPainter painter(universe->rowCount(), universe->colCount());
    painter.clear();
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
            painter.paint(pos.first - min_row + 2, pos.second - min_col + 2, '*', Color::green); // translate to top left with a margin
        }
        // print row offset
        std::string min_row_str = std::to_string(min_row);
        for (size_t i = 0; i < min_row_str.size(); ++i) {
            painter.paint(i + 2, 1, static_cast<char>(min_row_str[i]), Color::yellow);
        }
        // print col offset
        std::string min_col_str = std::to_string(min_col);
        for (size_t i = 0; i < min_col_str.size(); ++i) {
            painter.paint(1, i + 2, static_cast<char>(min_col_str[i]), Color::yellow);
        }
        universe->advance();
        std::this_thread::sleep_for(m_refresh_period);
        painter.shiftCursor(max_row - min_row + 2, max_col - min_col + 2); // (min_row, min_col) translated to (2, 2)
        painter.clear();
    }
    painter.shiftCursor(max_row - min_row + 1, 1);
}
