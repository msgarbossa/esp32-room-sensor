#ifndef TASK_HOME_ASSISTANT
#define TASK_HOME_ASSISTANT

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "../config/config.h"

// WiFiClientSecure MQTT_net;
WiFiClient MQTT_net;
PubSubClient MQTT_client(MQTT_net);

extern float temp;
extern float humidity;
extern float pressure;
extern int8_t wifi_strength;

void mqttConnect() {

    if(!WiFi.isConnected()){
        Serial.println("[MQTT] Failed.  WiFi not connected.");
        return;
    }

    Serial.println(F("[MQTT] Connecting to HA..."));
    MQTT_client.setServer(MQTT_HOST, MQTT_PORT);

    long startAttemptTime = millis();

    while (!MQTT_client.connect(DEVICE_NAME, MQTT_USER, MQTT_PASSWORD) &&
        millis() - startAttemptTime < MQTT_CONNECT_TIMEOUT) {
        delay(MQTT_CONNECT_DELAY);
    }

    if(!MQTT_client.connected()){
        Serial.println("[MQTT] Failed.  Connection attemp timeout.");
    }

    unsigned long MqttConnectTime = millis() - startAttemptTime;
    Serial.println("[MQTT] Connected (" + String(MqttConnectTime) + "ms)!");
}


void sendTempToMQTT(){

    if(!MQTT_client.connected()){
        Serial.println(F("[MQTT] Connection failed for temperature."));
        return;
    }

    if(!MQTT_client.connected()){
        Serial.println(F("[MQTT] Can't send to HA without MQTT. Abort."));
        return;
    }

    // variable for MQTT payload
    char msg[50];
    sprintf(msg, "{\"t\":\"%0.2f\",\"h\":\"%0.2f\",\"p\":\"%0.2f\",\"s\":\"%d\"}", temp, humidity, pressure, wifi_strength);

    Serial.print(F("[MQTT] HA publish: "));
    Serial.println(msg);

    MQTT_client.publish("home/" DEVICE_NAME "/temp", msg);
}

#endif
