#pragma once
#include <Arduino.h>
#include <Ticker.h>

namespace sumobot
{
    class LineSensor
    {
    public:
        LineSensor(unsigned int ioPhotoTransistor, unsigned int ioAnalogInput, unsigned int threshold);
        void init();
        uint16_t getValue();
        bool isLine();

    private:
        void updateValue();
        
        Ticker m_ticker;

        unsigned int m_ioPhotoTransistor;
        unsigned int m_ioAnalogInput;
        unsigned int m_threshold;
        uint16_t m_value;
    };

}