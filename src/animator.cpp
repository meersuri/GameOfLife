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

void Animator::paintLeftMargin(size_t row_count, size_t thickness, Color color) {
    for (size_t row = 0; row < row_count; ++row) {
        for (size_t col = 0; col < thickness; ++col) {
            m_painter.paint(row, col, "█", color);
        }
    }
}

void Animator::paintTopMargin(size_t col_count, size_t thickness, Color color) {
    for (size_t col = 0; col < col_count; ++col) {
        for (size_t row = 0; row < thickness; ++row) {
            m_painter.paint(row, col, "█", color);
        }
    }
}

void Animator::paintRightMargin(size_t start_col, size_t row_count, size_t thickness, Color color) {
    for (size_t row = 0; row < row_count; ++row) {
        for (size_t col = start_col; col < thickness + start_col; ++col) {
            m_painter.paint(row, col, "█", color);
        }
    }
}

void Animator::paintBottomMargin(size_t start_row, size_t col_count, size_t thickness, Color color) {
    for (size_t col = 0; col < col_count; ++col) {
        for (size_t row = start_row; row < thickness + start_row; ++row) {
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
            max_row = std::max(row, max_row);
            max_col = std::max(col, max_col);
        }
        paintLeftMargin(max_row + 1 + margin_thickness, margin_thickness, Color::red);
        paintTopMargin(max_col + 1 + margin_thickness, margin_thickness, Color::red);
        printRowOffset(0); // row, col offset is always zero
        printColOffset(0);
        for (const auto& [row, col]: universe->getAliveCellsPos()) {
            m_painter.paint(row + margin_thickness, col + margin_thickness, "█", Color::green); // add margins
        }
        std::this_thread::sleep_for(m_refresh_period);
        universe->advance();
        m_painter.shiftCursor(max_row + margin_thickness, max_col + margin_thickness); // since clear is from current cursor pos up, track max row col
        m_painter.clear();
    }
    m_painter.shiftCursor(max_row + margin_thickness, max_col + margin_thickness);
    m_painter.clear();
    m_painter.shiftCursor(max_row + margin_thickness + 1, 0);
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
        std::vector<std::pair<size_t, size_t>> alive_cells_pos = universe->getAliveCellsPos();
        for (const auto& [row, col]: alive_cells_pos) {
            min_row = std::min(static_cast<int64_t>(row), min_row); // safe cast: max pos 2**32
            max_row = std::max(row, max_row);
            min_col = std::min(static_cast<int64_t>(col), min_col); // safe cast: max pos 2**32
            max_col = std::max(col, max_col);
        }
        paintLeftMargin(max_row - min_row + 1 + margin_thickness, margin_thickness, min_col == 0 ? Color::red : Color::blue);
        paintTopMargin(max_col - min_col + 1 + margin_thickness, margin_thickness, min_row == 0 ? Color::red : Color::blue);
        printRowOffset(min_row);
        printColOffset(min_col);
        for (const auto& [row, col]: alive_cells_pos) {
            size_t cell_row = row - min_row + margin_thickness;
            size_t cell_col = col - min_col + margin_thickness;
            m_painter.paint(cell_row, cell_col, "█", Color::green); // translate to top left with a margin
        }
        std::this_thread::sleep_for(m_refresh_period);
        universe->advance();
        m_painter.shiftCursor(max_row - min_row + margin_thickness, max_col - min_col + margin_thickness);
        m_painter.clear();
    }
    m_painter.shiftCursor(max_row - min_row + margin_thickness, max_col - min_col + margin_thickness);
    m_painter.clear();
    m_painter.shiftCursor(max_row - min_row + 1, 0);
}


// Calculates mid-point of live-cells and centers the viewport on it
CenterAutoPanAnimator::CenterAutoPanAnimator(std::chrono::milliseconds refresh_period): Animator(refresh_period) {}

void CenterAutoPanAnimator::animate(Universe* universe, size_t time_steps) {
    size_t max_row = 0;
    size_t max_col = 0;
    size_t margin_thickness = 1;
    int64_t min_row = universe->rowCount();
    int64_t min_col = universe->colCount();
    size_t row_count = universe->rowCount();
    size_t col_count = universe->colCount();
    size_t viewport_rows = 40;
    size_t viewport_cols = 40;

    m_painter.clear();
    for (size_t i = 0; i < time_steps; ++i) {
        max_row = 0;
        max_col = 0;
        min_row = row_count;
        min_col = col_count;
        std::vector<std::pair<size_t, size_t>> alive_cells_pos = universe->getAliveCellsPos();
        double mid_row = 0.0;
        double mid_col = 0.0;
        for (const auto& [row, col]: alive_cells_pos) {
            mid_row += row;
            mid_col += col;
            min_row = std::min(static_cast<int64_t>(row), min_row); // safe cast: max pos 2**32
            max_row = std::max(row, max_row);
            min_col = std::min(static_cast<int64_t>(col), min_col); // safe cast: max pos 2**32
            max_col = std::max(col, max_col);
        }
        mid_row /= alive_cells_pos.size();
        mid_col /= alive_cells_pos.size();
        size_t viewport_top_row = std::max(0.0, mid_row - viewport_rows / 2);
        size_t viewport_bot_row = std::min(static_cast<double>(universe->rowCount()) - 1, mid_row + viewport_rows / 2);
        size_t viewport_left_col = std::max(0.0, mid_col - viewport_cols / 2);
        size_t viewport_right_col = std::min(static_cast<double>(universe->colCount()) - 1, mid_col + viewport_cols / 2);
        paintLeftMargin(viewport_rows + margin_thickness, margin_thickness, viewport_left_col == 0 ? Color::red : Color::blue);
        paintTopMargin(viewport_cols + 2 * margin_thickness, margin_thickness, viewport_top_row == 0 ? Color::red : Color::blue);
        paintRightMargin(viewport_cols + 2 * margin_thickness, viewport_rows + 2 * margin_thickness, margin_thickness, viewport_right_col == col_count - 1 ? Color::red : Color::blue);
        paintBottomMargin(viewport_rows + margin_thickness, viewport_cols + 2 * margin_thickness, margin_thickness, viewport_bot_row == row_count - 1 ? Color::red : Color::blue);
        printRowOffset(viewport_top_row);
        printColOffset(viewport_left_col);
        for (const auto& [row, col]: alive_cells_pos) {
            if (row < viewport_top_row || row > viewport_bot_row) {
                continue;
            }
            if (col < viewport_left_col || col > viewport_right_col) {
                continue;
            }
            size_t cell_row = row - viewport_top_row + margin_thickness;
            size_t cell_col = col - viewport_left_col + margin_thickness;
            m_painter.paint(cell_row, cell_col, "█", Color::green); // translate to top left with a margin
        }
        std::this_thread::sleep_for(m_refresh_period);
        universe->advance();
        m_painter.shiftCursor(max_row - viewport_top_row + margin_thickness, max_col - viewport_left_col + margin_thickness);
        m_painter.clear();
    }
    m_painter.shiftCursor(viewport_rows + margin_thickness, viewport_cols + margin_thickness);
    m_painter.clear();
    m_painter.shiftCursor(viewport_rows + margin_thickness + 1, 0);
}
