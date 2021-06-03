#include "Battery.h"
#include <Arduino.h>
#include <Log.h>
namespace sumobot
{

    Battery::Battery(unsigned int ioVoltage, unsigned int ioIsCharging)
        : m_ioVoltage(ioVoltage),
          m_ioIsCharging(ioIsCharging),
          m_voltage(0),
          m_stateOfCharge(0)
    {
    }

    void Battery::init()
    {
        // Setup battery charge detection pin
        pinMode(m_ioIsCharging, INPUT);
        // set analog input for battery voltage
        analogSetAttenuation(ADC_11db);
        adcAttachPin(m_ioVoltage);

        m_ticker.attach(
            5,
            +[](Battery *instance)
            { instance->updateValue(); },
            this);
        // force first update
        updateValue();
    }

    void Battery::updateValue()
    {

        // Don't update battery level when robot is moving
        // the servo motors lower the battery voltage
        // TODO: wait still a little more after moving
        // for the voltage to settle
        //if (robotMoving)
        //{
        //  return;
        //}
        // TODO how to check that robot is actually moving
        // Calculate the battery voltage
        float batteryVoltage = 2.12 * (analogRead(32) * 3.3 / 4096);
        // Calculate battery percentage
        int tempBatteryLevel = 0.0 + ((100.0 - 0.0) / (4.2 - 3.2)) * (batteryVoltage - 3.2);

        if (tempBatteryLevel > 100)
        {
            tempBatteryLevel = 100;
        }
        if (tempBatteryLevel < 0)
        {
            tempBatteryLevel = 0;
        }
        // When battery level changed more than 3%
        if (abs(m_stateOfCharge - tempBatteryLevel) > 3)
        {
            // Update battery level
            m_stateOfCharge = tempBatteryLevel;
        }
        // TODO add back the bluetooth stuff
        // Notify the new battery level
        //batteryLevelCharacteristic->setValue(&batteryLevel, 1);
        //batteryLevelCharacteristic->notify();
        Log::debugf("Voltage: %.2f, SoC: %d", batteryVoltage, m_stateOfCharge);
    }
    bool Battery::isCharging()
    {
        return digitalRead(m_ioIsCharging) > 0;
    }
}