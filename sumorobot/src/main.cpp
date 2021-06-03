/*
    This the code that runs on the SumoRobots
*/
// Include other libraries
#include <string.h>
#include <Ticker.h>
#include <NewPing.h>
#include <Arduino.h>
#include <Preferences.h>
#include <Log.h>
#include <SerialLogger.h>

#include "platform/platform.h"
#include "platform/UltrasonicSensor.h"
#include "platform/LineSensor.h"
#include "platform/Led.h"
#include "platform/Battery.h"

#include <WiFi.h>
#include <EStopReceiver.h>

#define DEBUG true

// Create preferences persistence
Preferences preferences;

// Create timers
Ticker sonarTimer;
Ticker batteryTimer;
Ticker connectionLedTimer;

// Other sensor stuff
uint8_t sensorValues[6];
bool ledFeedbackEnabled = true;

// Move command names
std::string cmdStop("stop");
std::string cmdLeft("left");
std::string cmdRight("right");
std::string cmdForward("forward");
std::string cmdBackward("backward");
// Other command names
std::string cmdLed("led");
std::string cmdLine("line");
std::string cmdName("name");
std::string cmdSonar("sonar");
std::string cmdServo("servo");
std::string cmdLedFeedback("ledf");

// device stuff
sumobot::Platform g_platform;

const static unsigned int IO_BIG_BLUE_LED = 16;
const static unsigned int IO_YELLOW_LED_LEFT = 17;
const static unsigned int IO_YELLOW_LED_RIGHT = 12;
const static unsigned int IO_CONNECTION_LED = 5;

sumobot::Led g_bigBlueLed(IO_BIG_BLUE_LED);
sumobot::Led g_yellowLedLeft(IO_YELLOW_LED_LEFT);
sumobot::Led g_yellowLedRight(IO_YELLOW_LED_RIGHT);
sumobot::Led g_connectionLed(IO_CONNECTION_LED, true);

const static unsigned int IO_ULTRASONIC_SENSOR_TRIGGER = 27;
const static unsigned int IO_ULTRASONIC_SENSOR_ECHO = 14;
sumobot::UltrasonicSensor g_ultrasonicSensor(IO_ULTRASONIC_SENSOR_TRIGGER, IO_ULTRASONIC_SENSOR_ECHO);

const static unsigned int IO_IS_CHARGING = 25;
const static unsigned int IO_BATTERY_VOLTAGE = 32;
sumobot::Battery g_battery(IO_BATTERY_VOLTAGE, IO_IS_CHARGING);

const static unsigned int LINE_THRESHOLD = 1000;
const static unsigned int IO_LINE_PHOTO_TRANSISTOR_LEFT = 19;
const static unsigned int IO_LINE_ANALOG_LEFT = 34;
sumobot::LineSensor g_lineLeft(IO_LINE_PHOTO_TRANSISTOR_LEFT, IO_LINE_ANALOG_LEFT, LINE_THRESHOLD);

const static unsigned int IO_LINE_PHOTO_TRANSISTOR_RIGHT = 23; // TODO check if this is really right or left
const static unsigned int IO_LINE_ANALOG_RIGHT = 33;
sumobot::LineSensor g_lineRight(IO_LINE_PHOTO_TRANSISTOR_RIGHT, IO_LINE_ANALOG_RIGHT, LINE_THRESHOLD);


uint8_t WIFI_CHANNEL = 0;

// id of the cell to which the station and the wireless estop belong
// this allows to have multiple setups belonging to different scopes
// e.g. Wireless estop with cell id 1 will stop all master stations
// that are set to id 1, however master stations set to other ids
// will drop and ignore the message.
uint8_t CELL_ID = 10;

// how long we wait between sending messages
unsigned long MSG_TIME_MS = 20;

// how many messages we can lose before activating E-Stop because of
// message timeout
unsigned long SKIP_BEFORE_TIMEOUT = 5;

// Address of the client station (wireless estop button)
uint8_t BUTTON_STATION_MAC[] = {0xDE, 0xAD, 0x13, 0x37, 0x00, 0x02};

estop::EStopReceiver *g_eStopReceiver;

estop::EStopState g_previousEStopState;

void updateSensorFeedback()
{
  g_bigBlueLed.set(g_ultrasonicSensor.isObstacleClose());
  g_yellowLedLeft.set(g_lineLeft.isLine());
  g_yellowLedRight.set(g_lineRight.isLine());
}

void updateSensorValues()
{
  uint16_t leftLineValue = g_lineLeft.getValue();
  uint16_t rightLineValue = g_lineRight.getValue();
  if (ledFeedbackEnabled)
    updateSensorFeedback();
  sensorValues[0] = g_ultrasonicSensor.getDistance();
  sensorValues[1] = leftLineValue >> 8;
  sensorValues[2] = leftLineValue;
  sensorValues[3] = rightLineValue >> 8;
  sensorValues[4] = rightLineValue;
  sensorValues[5] = digitalRead(25);
}

void blinkConnectionLed()
{
  g_connectionLed.set(true);
  delay(20);
  g_connectionLed.set(false);
}

void setup()
{

// delete old wifi settings
 // delete old wifi settings
  WiFi.disconnect();

  WiFi.mode(WIFI_STA); // Place in station mode for ESP-NOW sender node
  //disable sleep mode
  //WiFi.setSleepMode(WIFI_NONE_SLEEP);

  g_eStopReceiver = new estop::EStopReceiver(BUTTON_STATION_MAC, WIFI_CHANNEL, CELL_ID, MSG_TIME_MS * SKIP_BEFORE_TIMEOUT);
  g_eStopReceiver->init();
  g_previousEStopState = g_eStopReceiver->getEStopState();
  Serial.begin(115200);
  Log::init(new SerialLogger());

#if DEBUG
  Serial.println("Waiting a client connection to notify...");
#endif

  // Setup sonar timer to update it's value
  sonarTimer.attach_ms(50, updateSensorValues);

  g_platform.init();
  g_ultrasonicSensor.init();
  g_bigBlueLed.init();
  g_yellowLedLeft.init();
  g_yellowLedRight.init();
  g_connectionLed.init();
  g_battery.init();
  g_lineLeft.init();
  g_lineRight.init();
  connectionLedTimer.attach_ms(2000, blinkConnectionLed);

  pinMode(26, OUTPUT);
  digitalWrite(26, HIGH);
}

void cycleTest()
{
  for (int i = 60; i <= 100; i += 1)
  {
    // TODO: some hacky stuff
    //g_platform.setLeft(i * 0.1);
    Log::infof("Setting cycle to %d", i);
    g_platform.setCycleLeft(i);
    g_platform.setCycleRight(i);
    delay(5000);
  }
}

void velTest()
{
  for (int i = 0; i <= 10; i++)
  {
    // TODO: some hacky stuff
    //g_platform.setLeft(i * 0.1);
    Log::infof("Setting cycle to %.2f", i * 0.05);
    g_platform.setLeft(i * 0.1);
    g_platform.setRight(i * 0.1);
    delay(5000);
  }

  for (int i = 0; i >= -10; i--)
  {
    // TODO: some hacky stuff
    //g_platform.setLeft(i * 0.1);
    Log::infof("Setting cycle to %.2f", i * 0.05);
    g_platform.setLeft(i * 0.1);
    g_platform.setRight(i * 0.1);
    delay(5000);
  }
}

bool on = false;
void loop()
{
  // When BLE got disconnected
  /*
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);                    // Give the bluetooth stack the chance to get things ready
    bleServer->startAdvertising(); // Restart advertising
#if DEBUG
    Serial.println("start advertising");
#endif
    oldDeviceConnected = deviceConnected;
    connectionLedTimer.attach_ms(2000, blinkConnectionLed);
  }
  // When BLE got connected
  if (deviceConnected && !oldDeviceConnected)
  {
    oldDeviceConnected = deviceConnected;
    connectionLedTimer.detach();
    digitalWrite(5, LOW);
  }
  */
 if(g_eStopReceiver->getEStopState() == estop::ESTOP_FREE)
 {
   g_platform.setLeft(1. );
   g_platform.setRight(1.0);
 }
 else
 {
    g_platform.setLeft(0);
   g_platform.setRight(0);
 }
  delay(50);
}