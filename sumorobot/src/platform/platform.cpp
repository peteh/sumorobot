#include "platform.h"
#include <Log.h>
namespace sumobot
{
    Platform::Platform()
        : m_leftDutyCycle(0),
          m_rightDutyCycle(0),
          m_cycleCenter(71), // original 77
          m_cycleWidth(20)   // theoretcially 57.5
    {
    }

    void Platform::init()
    {
        m_leftDutyCycle = 0;
        m_rightDutyCycle = 0;

        // Setup the left servo PWM
        ledcSetup(CHANNEL_PWM_LEFT_DRIVE, 50, 10);
        ledcAttachPin(IO_LEFT_DRIVE, CHANNEL_PWM_LEFT_DRIVE);

        // Setup the right servo PWM
        ledcSetup(CHANNEL_PWM_RIGHT_DRIVE, 50, 10);
        ledcAttachPin(IO_RIGHT_DRIVE, CHANNEL_PWM_RIGHT_DRIVE);
    }

    void Platform::setCycleLeft(uint32_t cycle)
    {
        ledcWrite(CHANNEL_PWM_LEFT_DRIVE, cycle);
    }

    void Platform::setCycleRight(uint32_t cycle)
    {
        ledcWrite(CHANNEL_PWM_RIGHT_DRIVE, cycle);
    }

    float Platform::roundToStep(float f, uint steps)
    {
        return floor((f * steps) + 0.5) / steps;
    }

    void Platform::setLeft(float velFactor)
    {
        int cycle = 0;

        //hobby servos: 40 to 115, centre 77

        float speed = velFactor * m_cycleWidth; // (115 - 40) / 2
        if (velFactor != 0)
        {
            cycle = (speed + m_cycleCenter);
        }

        if (cycle == m_leftDutyCycle)
        {
            return;
        }
        m_leftDutyCycle = cycle;
        ledcWrite(CHANNEL_PWM_LEFT_DRIVE, m_leftDutyCycle);
    }

    void Platform::setRight(float velFactor)
    {
        int cycle = 0;

        //hobby servos: 40 to 115, centre 77

        float speed = -1. * velFactor * m_cycleWidth;
        if (velFactor != 0)
        {
            cycle = (speed + m_cycleCenter);
        }

        if (cycle == m_rightDutyCycle)
        {
            return;
        }
        m_rightDutyCycle = cycle;
        ledcWrite(CHANNEL_PWM_RIGHT_DRIVE, m_rightDutyCycle);
    }
}