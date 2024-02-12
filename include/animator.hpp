#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include <chrono>

class Universe; // forward declare

class Animator {
    public:
        Animator(std::chrono::milliseconds refresh_period);
        virtual void animate(Universe* universe, size_t time_steps) = 0;
    protected:
        std::chrono::milliseconds m_refresh_period;
        size_t m_time_steps;
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

