#pragma once
#include <Ticker.h>
#include <NewPing.h>

namespace sumobot
{
    class UltrasonicSensor
    {
    public:
        UltrasonicSensor(unsigned int ioTrigger, unsigned int ioEcho);
        void init();
        void setThreshold(uint8_t threshold);
        uint8_t getDistance();
        bool isObstacleClose();

    private:
        void updateValue();

        Ticker m_ticker;

        unsigned int m_ioTrigger;
        unsigned int m_ioEcho;

        uint8_t m_distance;
        NewPing m_sonar;
        uint8_t m_sonarThreshold;
    };
}