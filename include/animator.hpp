#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include <chrono>

#include "painter.hpp"

class Universe; // forward declare

class Animator {
    public:
        Animator(std::chrono::milliseconds refresh_period);
        virtual void animate(Universe* universe, size_t time_steps) = 0;
    protected:
        void printRowOffset(size_t offset, Color color=Color::yellow);
        void printColOffset(size_t offset, Color color=Color::yellow);
        std::chrono::milliseconds m_refresh_period;
        size_t m_time_steps;
        GridPainter m_painter;
};

class FullViewAnimator: public Animator {
    public:
        FullViewAnimator(std::chrono::milliseconds refresh_period);
        void animate(Universe* universe, size_t time_steps) override;
};

class AutoPanAnimator: public Animator {
    public:
        AutoPanAnimator(std::chrono::milliseconds refresh_period);
        void animate(Universe* universe, size_t time_steps) override;
};

#endif

