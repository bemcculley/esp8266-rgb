#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 4

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, PIN, NEO_GRB + NEO_KHZ800);



const char* ssid = "ChickenRings";
const char* password = "rossperotsdirtylittlesecret";

ESP8266WebServer server(80);

const int led = 5;

int r = 0;
int g = 0;
int b = 0;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleOff() {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
  server.send(200, "text/plain", "Leds Off");
}

void handleLed() {
  String message = "";
  if (server.arg("RGB") == "") {   //Parameter not found
    message = "RGB Argument not found";
  } else {

  if (server.arg("R") == "") {   //Parameter not found
    message = "R Argument not found";
  } else {    //Parameter found
    message = "R Argument = ";
    message += server.arg("R");
    message = "\n";
    r = server.arg("R").toInt(); //Gets the value of the query parameter
  }
  if (server.arg("G") == "") {   //Parameter not found
    message = "G Argument not found";
  } else {    //Parameter found
    message += "G Argument = ";
    message += server.arg("G");
    message += "\n";
    g = server.arg("G").toInt(); //Gets the value of the query parameter
  }
    if (server.arg("B") == "") {   //Parameter not found
    message = "B Argument not found";
  } else {    //Parameter found
    message += "B Argument = ";
    message += server.arg("B");
    message = "\n";
    b = server.arg("B").toInt(); //Gets the value of the query parameter
  }
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
    
  }
  strip.show();
  server.send(200, "text/plain", message);
  digitalWrite(led, 0);
  }
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("tv_leds")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/off", handleOff); 
  server.on("/Led", handleLed);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
