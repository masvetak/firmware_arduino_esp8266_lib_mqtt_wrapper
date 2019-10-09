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
        MQTTWrapper() {
            numOfConnectRetry = 10;
        }

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

        void initialize(const char *mqttServer, const int mqttPort, const char *mqttUser, const char *mqttPassword) {
            Serial.printf("%s: initialize...\n\r", FILE_NAME_MQTT_WRAPPER);

            _mqttServer = mqttServer;
            _mqttPort = mqttPort;
            _mqttUser = mqttUser;
            _mqttPassword = mqttPassword;

            _mqttClient.setCallback([this] (char* topic, uint8_t* payload, uint16_t length) { this->onMessageReceivedCallback(topic, payload, length); });
        }

        void setClient(Client &client) {
            _mqttClient.setClient(client);
        }

        void setServer(const char *mqttServer) {
            _mqttServer = mqttServer;
        }

        void setPort(const int mqttPort) {
            _mqttPort = mqttPort;
        }

        void setUser(const char *mqttUser) {
            _mqttUser = mqttUser;
        }

        void setPassword(const char *mqttPassword) {
            _mqttPassword = mqttPassword;
        }

        void setMainTopic(char *topic) {
            _topic = topic;
        }

        void setSubTopic(void (*onCallback)(uint8_t *payload, uint16_t length), const char *newTopic) {
            uint8_t index;
            uint8_t topicSize = sizeof(_topicList[_topicCount].topicLevel);
            bool topicExisted = false;

            if(_topicCount == _maxTopics) {
                Serial.printf("%s: register Topic list full!\n\r", FILE_NAME_MQTT_WRAPPER);
                return;
            }

            for (index = 0; index < _topicCount; index++) {
                if (!memcmp(&newTopic[0], &_topicList[index].topicLevel[0], topicSize - 1)) {
                    topicExisted = true;
                    break;
                }
            }

            _topicList[index].callback = onCallback;

            memcpy(&_topicList[index].topicLevel[0], &newTopic[0], topicSize);
            _topicList[index].topicLevel[topicSize - 1] = '\0';

            strcat(_topicList[index].topic, _topic);
            strcat(_topicList[index].topic, newTopic);

            if (!topicExisted) {
                ++_topicCount;
                // Serial.printf("%s: Topic registed\n\r", FILE_NAME_MQTT_WRAPPER);
            }
        }

        void publish(const char* topic, const char* message) {
            _mqttClient.publish(topic, message);
        }

        void connect() {
            _mqttClient.setServer(_mqttServer, _mqttPort);    
        }

        void disconnect() {
            if(_mqttClient.connected()) {
                _mqttClient.disconnect();
            }
        }

        bool isConnected() {
            if(_mqttClient.connected()) {
                return true;
            } else {
                return false;
            }
        }

        void printConnectionFailedState() {
            switch(_mqttClient.state()) {
                case -4:
                    Serial.printf("%s: connection timeout\n\r", FILE_NAME_MQTT_WRAPPER);
                break;

                case -3:
                    Serial.printf("%s: connection lost\n\r", FILE_NAME_MQTT_WRAPPER);
                break;

                case -2:
                    Serial.printf("%s: connect failed\n\r", FILE_NAME_MQTT_WRAPPER);
                break;

                case -1:
                    Serial.printf("%s: disconnection\n\r", FILE_NAME_MQTT_WRAPPER);
                break;
                
                case 1:
                    Serial.printf("%s: connect bad protocol\n\r", FILE_NAME_MQTT_WRAPPER);
                break;

                case 2:
                    Serial.printf("%s: connect bad client ID\n\r", FILE_NAME_MQTT_WRAPPER);
                break;

                case 3:
                    Serial.printf("%s: connection unavailable\n\r", FILE_NAME_MQTT_WRAPPER);
                break;

                case 4:
                    Serial.printf("%s: connect bad credentials\n\r", FILE_NAME_MQTT_WRAPPER);
                break;

                case 5:
                    Serial.printf("%s: connect unauthorized\n\r", FILE_NAME_MQTT_WRAPPER);
                break;               
            }
        }

        void threadLoop() {
            _mqttClient.loop();
        }

        void onMessageReceivedCallback(char* topic, uint8_t* payload, uint16_t length) {

            uint8_t topicSize = sizeof(_topicList[_topicCount].topic);
            
            Serial.printf("%s: onMessageReceivedCallback [%s] [", FILE_NAME_MQTT_WRAPPER, topic);
            for (int i=0;i<length;i++) {
                Serial.print((char)payload[i]);
            }
            Serial.println("]");

            for (uint8_t index = 0; index < _topicCount; index++) {
                TopicList *ref = &_topicList[index];

                if (!memcmp(&topic[0], &_topicList[index].topic[0], topicSize - 1)) {
                    if (ref->callback != nullptr) {
                        ref->callback(payload, length);
                        break;
                    } else {
                        Serial.printf("%s: topic callback is null!\n\r", FILE_NAME_MQTT_WRAPPER);
                        break;
                    }
                }
            }
        }

        static void staticTickerCallback() {
            getInstance().tickerCallbackFunction();
        }

        void tickerCallbackFunction() {
            if(numOfConnectRetry) {
                if(!_mqttClient.connected()) {
                    Serial.printf("%s: connecting to %s...\n\r", FILE_NAME_MQTT_WRAPPER, _mqttServer); 
                    if(_mqttClient.connect("SmartHomeLight", _mqttUser, _mqttPassword )) {
                        Serial.printf("%s: connected to server: %s\n\r", FILE_NAME_MQTT_WRAPPER, _mqttServer);
                        Serial.printf("%s: with user: %s\n\r", FILE_NAME_MQTT_WRAPPER, _mqttUser);
                    
                        for(uint8_t index = 0; index < _topicCount; index++)
                        {
                            _mqttClient.subscribe(_topicList[index].topic);
                            Serial.printf("%s: subscribed to topic: %s\n\r", FILE_NAME_MQTT_WRAPPER, _topicList[index].topic);
                        }
                        _mqttClient.publish("sss/shs/5C:CF:7F:68:13:FB/info", "boot");
                    } else {
                        Serial.printf("%s: failed with state %d \n\r", FILE_NAME_MQTT_WRAPPER, _mqttClient.state());
                        printConnectionFailedState();
                    } 
                } else {
                    if(_mqttClient.state() != 0) {
                        numOfConnectRetry--;
                    }
                }
            } else {
                Serial.printf("%s: connecting retry's expired...\n\r", FILE_NAME_MQTT_WRAPPER); 
            }
        }
};

/* Define to prevent recursive inclusion ------------------------------------ */
#endif //__MQTT_WRAPPER_HPP__