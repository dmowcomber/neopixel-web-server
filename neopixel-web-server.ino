#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WS2812FX.h>

#ifndef APSSID
#define APSSID "test"
#define APPSK  "testing123"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

#define pixelCount 7
#define pixelPin 5

ESP8266WebServer server(80);
WS2812FX ws2812fx(pixelCount, pixelPin, NEO_GRB + NEO_KHZ800);

void handleRoot() {
  server.send(200, "text/html", R"(
  <!DOCTYPE html><html><head>
    <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script>
    <script src='https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.0.0/js/bootstrap.min.js'></script>
    <script src='https://cdnjs.cloudflare.com/ajax/libs/jqueryui/1.12.1/jquery-ui.min.js'></script>
    <script type='text/javascript'>
      console.log("hellow from js");
      $(document).ready(function(){
        $("button").on('click', function(){
          console.log('click!');

          var button = $(this);
          console.log('1');

          curr = button.attr("value");
          console.log('2');

          console.log(curr);
          if (curr == 'color') {
            $.ajax({
              url: '/color',
              success: function(data){},
              error: function(error){
                console.log("error: " + JSON.stringify(error));
              }
            });
          } else if (curr == 'rainbow') {
            $.ajax({
              url: '/rainbow',
              success: function(data){},
              error: function(error){
                console.log("error: " + JSON.stringify(error));
              }
            });
          }
        });
      });
    </script>
  </head>
  <body>
    <h1>You are connected</h1>
    <button value='color' id='color'>color</button>
    <button value='rainbow' id='rainbow'>rainbow</button>
  </body></html>
  )");
}

void handleOrange() {
  uint32 color = 0xFF4301;
  ws2812fx.setColor(color);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.setBrightness(20);
  ws2812fx.service();

  String colorStr = String(color, HEX);
  colorStr.toUpperCase();

  String message = "color set to 0x" + colorStr;
  server.send(200, "text/html", message);
}

void handleMode() {
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

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  Serial.print("Initializing lights...");

  ws2812fx.init();
  ws2812fx.setSpeed(DEFAULT_SPEED);
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
  server.on("/color", handleOrange);
  server.on("/rainbow", handleMode);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
