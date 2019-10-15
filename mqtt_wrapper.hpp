/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __MQTT_WRAPPER_HPP__
#define __MQTT_WRAPPER_HPP__

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "app_common_constants.h"

struct TopicList {
    char topic[32];
    char topicLevel[16];
    void (*callback)(uint8_t *payload, uint16_t length);
};

class MQTTWrapper {

    private:
        // constructor
        MQTTWrapper();

        // singleton (C++ 03) - unimplemented methods
        MQTTWrapper(MQTTWrapper const&);
        void operator=(MQTTWrapper const&);

        // singleton (C++ 11) - deleted methods
        // public:
        // MQTTWrapper(MQTTWrapper const&) = delete;
        // void operator=(MQTTWrapper const&) = delete;

        PubSubClient _mqttClient;

        const char *_mqttServer;
        int         _mqttPort;
        const char *_mqttUser;
        const char *_mqttPassword;

        uint8_t numOfConnectRetry;

        char *_topic;

        static const uint8_t _maxTopics = 8;
        uint8_t _topicCount = 0;
        
        struct TopicList _topicList[_maxTopics];
    
    public:
        // singleton
        static MQTTWrapper& getInstance() {
            static MQTTWrapper instance;
            return instance;
        }

        void initialize(const char *mqttServer, const int mqttPort, const char *mqttUser, const char *mqttPassword);

        void setClient(Client &client);

        void setServer(const char *mqttServer);

        void setPort(const int mqttPort);

        void setUser(const char *mqttUser);

        void setPassword(const char *mqttPassword);

        void setMainTopic(char *topic);

        void setSubTopic(void (*onCallback)(uint8_t *payload, uint16_t length), const char *newTopic);

        void publish(const char* topic, const char* message);

        void connect();

        void disconnect();

        bool isConnected();

        void printConnectionFailedState();

        void threadLoop();

        void onMessageReceivedCallback(char* topic, uint8_t* payload, uint16_t length);

        static void staticTickerCallback();

        void tickerCallbackFunction();
};

/* Define to prevent recursive inclusion ------------------------------------ */
#endif //__MQTT_WRAPPER_HPP__