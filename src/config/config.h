
#ifndef CONFIG
#define CONFIG

#define DEVICE_NAME "room1"

// How often to wake up from deep sleep
#define TIME_TO_SLEEP_SEC 300

// WiFi
#define WIFI_NETWORK "SSID"
#define WIFI_PASSWORD "PW"
#define WIFI_TIMEOUT 20000 // 20 seconds
#define WIFI_RECOVER_TIME_MS 1000 // (1-20 seconds)

// MQTT
#define MQTT_HOST IPAddress(X, X, X, X)
#define MQTT_PORT 1883
#define MQTT_USER "user"
#define MQTT_PASSWORD "password"
#define MQTT_CONNECT_DELAY 200
#define MQTT_CONNECT_TIMEOUT 10000 // 10 seconds

#endif
