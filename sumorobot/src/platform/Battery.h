#pragma once
#include <Ticker.h>
namespace sumobot
{
    class Battery
    {
    public:
        Battery(unsigned int ioVoltage, unsigned int ioIsCharging);
        void init();

        bool isCharging();

    private:
        void updateValue();
        Ticker m_ticker;
        unsigned int m_ioVoltage;
        unsigned int m_ioIsCharging;

        float m_voltage;
        unsigned int m_stateOfCharge;
    };

}