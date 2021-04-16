# ESP32 room temp sensor

## Overview

This ESP32 sensor is setup to wake from deep sleep every 5 minutes, take a sensor reading, send the data to MQTT, and go back to sleep.  Intended for dark rooms, the OLED is only activated for about 10 seconds by tapping the touch sensor on the top.  The number in the lower-right corner is the signal strength.  Update settings in src/config/config.h.

![project photo](/img/ESP32-station-oled.jpg)

I resized this case to fit the parts I like to use.  The screen is currently just glued in with hot glue.

![front](/img/ESP32-station-front.jpg)

The 30-pin ESP32 board with integrated antenna slides perfectly into the rail.

![back](/img/ESP32-station-back.jpg)

The cover hooks into the top edge and the bottom can stay pressed in.  I didn't need to add screws.  I modified the cover with a small cutout at the top so the BME280 sensor can be external.  I'd like to further tweak the cover to better hold the sensor.

## Parts

- [ESP32 devkit - 30 pin, integrated antenna](https://www.aliexpress.com/item/1005001267643044.html)
- [0.96" OLED - white, I2C](https://www.aliexpress.com/item/32896971385.html)
- [BME280 sensor](https://www.aliexpress.com/item/4001098967210.html)


## Diagram

![diagram](/img/diagram.png)

## Node-RED

![Node-RED flow](/node-red/node-red-flow.png)
[JSON for above flow](node-red/flow.json)

The beginnig of the flow starts out very simple.  Node-RED listens on the MQTT topic for the sensor.  The messages get converted from JSON to payload objects.

To understand the rest of the flow, it's important to go over how the Prometheus node exporter works in Node-RED.  The orange metric boxes to the right-side of the flow generate the prometheus data shown below at \<node_red_url\>:1880/metrics.

```
# HELP room1_temp room1 temp
# TYPE room1_temp gauge
room1_temp 78

# HELP room1_humidity room1 humidity
# TYPE room1_humidity gauge
room1_humidity 31.32

# HELP room1_pressure room1 pressure
# TYPE room1_pressure gauge
room1_pressure 964.77

# HELP room1_signal room1 signal
# TYPE room1_signal gauge
room1_signal -70
```

The challenge has been dealing with sensors that go offline.  The metric node would not receive an updated value and continue to display an old value.  This would show up in the graphs in Grafana as a flat-line.  There might be a more elegant solution, but what I've done for now is to use an inject node to regularly inject a healthcheck message into the flow.  The healthcheck function node uses node context to store the time of the last valid message that came from MQTT.  With the healthcheck message is injected, the time of the inject message is compared to the time of the last valid message.  If the time difference is greater than the threshold, the healthcheck node's status is set to "offline" and a new message is generated with the metric values set to zero.  The Grafana graphs are set to only display values greater than zero.

The functions to the left of each of the orange metric nodes simply reformat the payload messages to set the metric values as required for the Promtheus metric nodes.

Below is an example graph in Grafana.

![Prometheus - Grafana](/node-red/prom-grafana.png)

## Over the Air (OTA) Updates

OTA updates seem more commonly used with sensors that do not utilize deep sleep.  To get OTA updates to work and minimize the wake time, the bootcount variable is used with RTC_DATA_ATTR to maintain the number of times the ESP32 wakes from sleep after being powered on.  The OTA update is only checked the first time the ESP32 is powered on.  The OTA update process works the same is it normally would except that the ESP32 needs a hard restart to reset the counter to zero and check for the update.  The IP address of the ESP32 needs to be updated in platform.ini file and the upload_port entry for USB needs to be commented out.
