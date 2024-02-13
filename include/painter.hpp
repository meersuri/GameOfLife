#ifndef PAINTER_HPP
#define PAINTER_HPP

#include <string>

enum class Color {
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
    blue = 34,
};

class GridPainter {
    public:
        GridPainter();
        template <typename T>
        void paint(size_t row, size_t col, T cell_char, Color color);
        template <typename T>
        void paint(T cell_char, Color color);
        void clear();
        void shiftCursor(size_t row, size_t col);
        ~GridPainter();
    private:
        inline static std::string m_esc{"\x1B["};
        inline static std::string m_reset_style{"\x1B[0m"};
};

#endif
