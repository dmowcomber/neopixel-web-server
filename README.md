# neopixel-web-server

A HTTP API and frontend to control neopixel lights. 

## Install
Compile and uplaod the Arduino script using the [Arduino-CLI](https://github.com/arduino/arduino-cli)
```
# install on a Wemos D1 Mini.
arduino-cli core update-index --additional-urls https://arduino.esp8266.com/stable/package_esp8266com_index.json
arduino-cli core update-index
# get the device for the Wemos D1 Mini
arduino-cli board list| grep usb
# /dev/cu.usbserial-14210         Serial Port (USB) Unknown
arduino-cli compile --fqbn esp8266:esp8266:d1_mini_lite
arduino-cli upload -p /dev/cu.usbserial-14210 --fqbn esp8266:esp8266:d1_mini_lite
```

## Use
Connect to the Wemos D1 Mini's wifi network using the [credentials](https://github.com/dmowcomber/neopixel-web-server/blob/master/neopixel-web-server.ino#L6-L10) defined in the Arduino Sketch. Change the credentials before compiling and uploading if you'd like to us different credentials.
Navigate to `http://192.168.4.1/` or curl the API directly:
```
# set brightness
curl "http://192.168.4.1/brightness?brightness=20" # brightness can be set anything from 0 to 255

# set color
curl "http://192.168.4.1/color?color=fc4605" # orange

# set mode
curl "http://192.168.4.1/mode?mode=2" # Breathing light mode. See WS2812FX Modes
```
For the /modes endpoint use the [WS2812FX Modes](https://github.com/kitesurfer1404/WS2812FX/blob/2b62056f6c6f90c94b9c2562fb2a32fa54abc382/src/modes_arduino.h#L40)
