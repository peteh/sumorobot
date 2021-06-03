#include "LineSensor.h"
#include <Log.h>
namespace sumobot
{
    LineSensor::LineSensor(unsigned int ioPhotoTransistor, unsigned int ioAnalogInput, unsigned int threshold)
        : m_ioPhotoTransistor(ioPhotoTransistor),
          m_ioAnalogInput(ioAnalogInput),
          m_threshold(threshold)
    {
    }

    void LineSensor::init()
    {

        // Phototransistor pull-ups
        pinMode(m_ioPhotoTransistor, INPUT_PULLUP);
        // Setup ADC for reading phototransistors
        adcAttachPin(m_ioAnalogInput);
        m_ticker.attach_ms(
            50, +[](LineSensor *instance)
                { instance->updateValue(); },
            this);
    }

    uint16_t LineSensor::getValue()
    {
        return m_value;
    }
    
    void LineSensor::updateValue()
    {
        m_value = analogRead(m_ioAnalogInput);
    }

    bool LineSensor::isLine()
    {
        return (m_value > m_threshold);
    }
}