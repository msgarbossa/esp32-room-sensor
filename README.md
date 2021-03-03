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

