#pragma once
#include <Arduino.h>

namespace sumobot
{
    const static unsigned int CHANNEL_PWM_LEFT_DRIVE = 1;
    const static unsigned int IO_LEFT_DRIVE = 15;

    const static unsigned int CHANNEL_PWM_RIGHT_DRIVE = 2;
    const static unsigned int IO_RIGHT_DRIVE = 4;

    class Platform
    {
        public: 
        Platform();
        void init();
        void setLeft(float velFactor);
        void setRight(float velFactor);

        void setCycleLeft(uint32_t cycle);
        void setCycleRight(uint32_t cycle);
        private: 

        int m_leftDutyCycle; 
        int m_rightDutyCycle;
        float m_cycleCenter;
        float m_cycleWidth;
    };
}
