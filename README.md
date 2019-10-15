# firmware_esp8266_arduino_lib_mqtt_wrapper
ESP8266 Adruino FrameworkMQTT Wrapper

|Public Method|Description|
|-------------|-----------|
|MQTTWrapper()|Constructor with client class|
|initialize()|Method for initialize credential's|
|setClient()|Metod for setting client|
|setServer()|Metod for setting servet|
|setPort()|Metod for setting server port|
|setUser()|Metod for setting user|
|setPassword()|Metod for setting password|
|setMainTopic()|Metod for setting main topic for subscribe|
|setSubTopic()|Metod for setting sub topic for subscribe|
|connect()|Implement this function for connecting to MQTT broker|
|disconnect()|Implement this function for disconnecting from MQTT broker|
|isConnected()|Returns connection state|
|onMessageReceivedCallback()|Implement this function for tiggering actions receving messages|
|threadLoop()|Implement this function in main thread for message receiving|
|staticTickerCallback()|Tick this function (1s-10s) for auto connection handling|
|printConnectionFailedState()|Returns uint32_t hexadecimal value that specifies frequency band|
