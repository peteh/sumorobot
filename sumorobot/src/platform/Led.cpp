#include "Led.h"
#include <Arduino.h>

namespace sumobot
{
    Led::Led(unsigned int io, bool reversed)
    : m_io(io),
    m_isOn(false),
    m_reversed(false)
    {

    }
    void Led::init()
    {
        pinMode(m_io, OUTPUT);
        m_isOn = false;
        digitalWrite(m_io, LOW);
    }

    void Led::set(bool state)
    {
        if(m_reversed)
        {
            state = !state;
        }

        if(m_isOn != state)
        {
            digitalWrite(m_io, state);
            m_isOn = state;
        }
    }

    void Led::on()
    {
        set(true);
    }

    void Led::off()
    {
        set(false);
    }


    void Led::flip()
    {
        set(!m_isOn);
    }
}