#include <ESP8266WebServer.h>
// #include <ESP8266WiFi.h> // already gets included through ESP8266WebServer
// #include <WiFiClient.h>  // already gets included through ESP8266WebServer
#include <WS2812FX.h>

/* Set these to your desired credentials. */
#ifndef APSSID
#define APSSID "test"
#define APPSK  "testing123"
#endif

const char *ssid = APSSID;
const char *password = APPSK;

#define pixelCount 7
#define pixelPin 5

ESP8266WebServer server(80);
WS2812FX ws2812fx(pixelCount, pixelPin, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("Initializing lights...");

  ws2812fx.init();
  ws2812fx.setColor(0xFF4301);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.setBrightness(20);
  ws2812fx.start();
  ws2812fx.show();
  ws2812fx.service();

  // when connecting through the soft AP should be accessible from 192.168.4.1
  Serial.print("Configuring access point...");
  Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
  WiFi.hostname("test");
  Serial.printf("New hostname: %s\n", WiFi.hostname().c_str());
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP SSID: ");
  Serial.println(APSSID);
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/color", handleColor);
  server.on("/rainbow", handleRainbow);
  server.on("/mode", handleMode);
  server.on("/orange", handleOrange);
  server.on("/brightness", handleBrightness);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // TODO: figure out how to do Arduino things asynchronously... or convert this to TinyGo when it supports goroutines and Wemos D1 Mini's Wi-Fi
  server.handleClient();
  ws2812fx.service();
}

void handleRoot() {
  server.send(200, "text/html", R"(
  <!DOCTYPE html><html><head>
    <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script>
    <script src='https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.0.0/js/bootstrap.min.js'></script>
    <script src='https://cdnjs.cloudflare.com/ajax/libs/jqueryui/1.12.1/jquery-ui.min.js'></script>
    <script type='text/javascript'>
      console.log("hellow from js");

      var callEndpoint = function(path) {
        $.ajax({
          url: path,
          success: function(data){
            $("#status").text(path + ' done');
          },
          error: function(error){
            $("#status").text('Error: ' + JSON.stringify(error));
          }
        });
      };

      $(document).ready(function(){
        $("button").on('click', function(){
          // var button = $(this); // TODO: update the button color to indicate that its active?
          var buttonID = this.id;
          if (buttonID == 'color') {
            var colorHex = $('#colorHex').val();
            callEndpoint('/color?color=' + colorHex);
          } else if (buttonID == 'rainbow') {
            callEndpoint('/rainbow')
          } else if (buttonID == 'brightness') {
            var brightnessInput = $('#brightnessInput').val();
            callEndpoint('/brightness?brightness=' + brightnessInput);
          } else if (buttonID == 'orange') {
            callEndpoint('/orange')
          }
        });
      });
    </script>
  </head>
  <body>
    <h1>You are connected</h1>
    <button id='color'>color</button>
    <input type="text" id="colorHex"><br />
    <button id='brightness'>brightness 0-255</button>
    <input type="text" id="brightnessInput"><br />
    <button id='rainbow'>rainbow</button>
    <button id='orange'>orange</button>
    <div id='status'></div>
  </body></html>
  )");
}

void handleOrange() {
  uint32 colorInt = 0xFF4301;
  ws2812fx.setColor(colorInt);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.setBrightness(20);
  ws2812fx.service();
  String message = "setting color hex: " + intToStrHex(colorInt);
  server.send(200, "text/html", message);
}

void handleColor() {
  uint32 colorInt = 0; // default to black
  // 0xFF4301

  String message = "default color is" + colorInt;

  String colorArg = server.arg("color");
  if (colorArg != "") {
    message = message + "<br> color arg is " + colorArg;
    // colorInt = (int) strtol(colorArg, NULL, 16);
    colorInt = strToHex(colorArg);
  }

  String colorHexStr = intToStrHex(colorInt);
  message = message + "<br> setting color - int: " + String(colorInt) + " hex: " + colorHexStr;

  ws2812fx.setColor(colorInt);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.setBrightness(20);
  ws2812fx.service();

  server.send(200, "text/html", message);
}

void handleBrightness() {
  uint8 brightness = 0;
  String brightnessArg = server.arg("brightness");
  brightness = brightnessArg.toInt();
  if (brightness < 0) {
    brightness = 0;
  }
  if (brightness > 255) {
    brightness = 255;
  }

  ws2812fx.setBrightness(brightness);
  ws2812fx.service();

  String message = "set brightness to " + String(brightness);
  server.send(200, "text/html", message);
}

void handleMode() {
  uint8 mode = 0;
  String modeArg = server.arg("mode");
  mode = modeArg.toInt();
  if (mode < 0) {
    mode = 0;
  }
  if (mode > 64) {
    mode = 64;
  }

  ws2812fx.setMode(mode);
  ws2812fx.show();
  ws2812fx.service();

  String message = "set mode to " + String(mode);
  server.send(200, "text/html", message);
}

void handleRainbow() {
  uint8 mode = FX_MODE_RAINBOW_CYCLE;

  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(5);
  ws2812fx.setMode(mode);
  ws2812fx.start();
  ws2812fx.show();
  ws2812fx.service();

  String message = "mode set to " + String(mode);
  // String message = "mode set to ";
  server.send(200, "text/html", message);
}

int strToHex(String str) {
  const char * c = str.c_str();
  return (int) strtol(c, 0, 16);
}

String intToStrHex(uint32 color) {
  String colorStr = String(color, HEX);
  colorStr.toUpperCase();
  return colorStr;
}
