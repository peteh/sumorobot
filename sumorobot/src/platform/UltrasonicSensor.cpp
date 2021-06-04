#include "UltrasonicSensor.h"
#include <Log.h>

namespace sumobot
{
    UltrasonicSensor::UltrasonicSensor(unsigned int ioTrigger, unsigned int ioEcho, uint8_t threshold)
        : m_ioTrigger(ioTrigger),
          m_ioEcho(ioEcho),
          m_distance(0),
          m_sonar(m_ioTrigger, m_ioEcho, 50),
          m_sonarThreshold(threshold)

    {
    }

    void UltrasonicSensor::init()
    {

        pinMode(m_ioTrigger, OUTPUT);
        pinMode(m_ioEcho, INPUT);

        // TODO: 50ms
        m_ticker.attach_ms(
            100, +[](UltrasonicSensor *instance)
                 { instance->updateValue(); },
            this);
    }

    void UltrasonicSensor::setThreshold(uint8_t threshold)
    {
        m_sonarThreshold = threshold;
    }

    void UltrasonicSensor::updateValue()
    {
        uint8_t distance = m_sonar.ping_cm();
        // When we didn't receive a ping back
        // set to max distance
        if (distance == 0)
        {
            distance = 255;
        }
        m_distance = distance;
        //Log::infof("Sonar distance: %d", m_distance);
    }

    uint8_t UltrasonicSensor::getDistance()
    {
        return m_distance;
    }

    bool UltrasonicSensor::isObstacleClose()
    {
        return m_distance < m_sonarThreshold;
    }
}