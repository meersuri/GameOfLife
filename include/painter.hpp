#ifndef PAINTER_HPP
#define PAINTER_HPP

#include <string>

enum class Color {
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
};

class GridPainter {
    public:
        GridPainter(size_t rows, size_t cols);
        void paint(size_t row, size_t col, char cell_char, Color color);
        void paint(char cell_char, Color color);
        void clear();
        void shiftCursor(size_t row, size_t col);
        ~GridPainter();
    private:
        size_t m_rows;
        size_t m_cols;
        inline static std::string m_esc{"\x1B["};
        inline static std::string m_reset_style{"\x1B[0m"};
};

#endif
