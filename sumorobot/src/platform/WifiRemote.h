#include <PubSubClient.h>
#include <WiFi.h>
#include "platform.h"
namespace sumobot
{
    class WifiRemote
    {
    public:
        WifiRemote();

        void init();
        void reconnect();
        void loop();

        bool getHonk();
        float getLeft();
        float getRight();

        bool getLeftLedOverride();
        bool getRightLedOverride();
        bool getMiddleLedOverride();

        bool getDistanceOverride();

    private:
        String composeClientID();
        String macToStr(const uint8_t *mac);
        void messageCallback(char *topic, byte *payload, unsigned int length);
        void handleMsgTimeout();

        WiFiClient m_wifiClient;
        PubSubClient m_mqttClient;

        bool m_honk;
        bool m_leftLedOverride;
        bool m_rightLedOverride;
        bool m_middleLedOverride;
        bool m_distanceOverride;
        float m_left;
        float m_right;
        long m_timestampLastMsg;
    };
}