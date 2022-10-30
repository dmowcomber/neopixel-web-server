# neopixel-web-server

A HTTP API and frontend to control neopixel lights. 

## Install
```
# install on a Wemos D1 Mini.
arduino-cli core update-index --additional-urls https://arduino.esp8266.com/stable/package_esp8266com_index.json
arduino-cli core update-index
arduino-cli compile --fqbn esp8266:esp8266:d1_mini_lite
sudo arduino-cli upload -p /dev/cu.usbserial-14210 --fqbn esp8266:esp8266:d1_mini_lite
```