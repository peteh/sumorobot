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

    private:
        String composeClientID();
        String macToStr(const uint8_t *mac);
        void messageCallback(char *topic, byte *payload, unsigned int length);

        WiFiClient m_wifiClient;
        PubSubClient m_mqttClient;

        bool m_honk;
        float m_left;
        float m_right;
    };
}