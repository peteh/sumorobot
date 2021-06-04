#include "WifiRemote.h"
#include <ArduinoJson.h>
#include <Log.h>

namespace sumobot
{
    //SSID of your network
    char ssid[] = "Drop it like it's Hotspot";
    //password of your WPA Network
    char pass[] = "fortunabier";
    WifiRemote::WifiRemote()
        : m_wifiClient(),
          m_mqttClient(m_wifiClient), 
          m_honk(false),
          m_left(0.f),
          m_right(0.f)
    {
    }
    // TODO: reconnect
    void WifiRemote::init()
    {
        WiFi.disconnect();
        WiFi.begin(ssid, pass);
        Log::info("Connecting");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Log::info(".");
        }
        Log::infof("Connected, IP address: %s", WiFi.localIP().toString().c_str());
        m_mqttClient.setServer("192.168.2.135", 1883);

        m_mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                                 { this->messageCallback(topic, payload, length); });
        reconnect();
    }

    bool WifiRemote::getHonk()
    {
        return m_honk;
    }

    float WifiRemote::getLeft()
    {
        return m_left;
    }

    float WifiRemote::getRight()
    {
        return m_right;
    }

    void WifiRemote::messageCallback(char *topic, byte *payload, unsigned int length)
    {
        //Log::info("Message received");
        StaticJsonDocument<256> doc;
        deserializeJson(doc, payload);
        float left = doc["left"]; 
        float right = doc["right"]; 
        bool honk = doc["honk"];
        m_left = left;
        m_right = right;
        m_honk = honk;
        Log::info(".");
    }

    String WifiRemote::macToStr(const uint8_t *mac)
    {
        String result;
        for (int i = 0; i < 6; ++i)
        {
            result += String(mac[i], 16);
            if (i < 5)
                result += ':';
        }
        return result;
    }
    String WifiRemote::composeClientID()
    {
        uint8_t mac[6];
        WiFi.macAddress(mac);
        String clientId;
        clientId += "esp-";
        clientId += macToStr(mac);
        return clientId;
    }
    void WifiRemote::loop()
    {
        m_mqttClient.loop();
    }

    void WifiRemote::reconnect()
    {
        // Loop until we're reconnected
        while (!m_mqttClient.connected())
        {
            Log::info("Attempting MQTT connection...");

            String clientId = composeClientID();
            clientId += "-";
            clientId += String(micros() & 0xff, 16); // to randomise. sort of

            // Attempt to connect
            if (m_mqttClient.connect(clientId.c_str()))
            {
                Log::info("connected");
                // Once connected, publish an announcement...
                //client.publish(ROOT_TOPIC.c_str(), ("connected " + composeClientID()).c_str() , true );
                // ... and resubscribe
                // topic + clientID + in
                String subscribeTopic = "remote";
                m_mqttClient.subscribe(subscribeTopic.c_str());
                Log::infof("subscribed to: %s", subscribeTopic.c_str());
            }
            else
            {
                Log::errorf("failed, rc=%d", m_mqttClient.state());
                Log::errorf(" wifi=%d", WiFi.status());
                Log::error(" try again in 5 seconds");
                // Wait 5 seconds before retrying
                delay(5000);
            }
        }
    }
}