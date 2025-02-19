/* Libraries */
#include <AIS.h>
#include <WiFi.h>
#include <SPI.h>
#include <Ethernet.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h> //credit goes to adafruit
#include <Adafruit_SSD1306.h> //credit goes to adafruit
// import AES encryption library
#include "AESLib.h"
// import base64 conversion library
#include "arduino_base64.hpp"
// declare a global AESLib object
AESLib aesLib;

/* Constants */
#define RXD2 16
#define TXD2 17
#define RXD1 32
#define TXD1 33
#define SCREEN_WIDTH 128 //OLED display width, in pixels
#define SCREEN_HEIGHT 64 //OLED display height, in pixels
#define MYIPADDR 192,168,1,28 //Static IP address
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,1,1
#define MYGW 192,168,1,1

const char * destination;
const char *name; 
char *rawAIS;
String lon, lat, sog, cog, eta, draught, gps;
unsigned int hdg, timeS, msgType;
long mmsi, imo;
int shiptype, connected = 0;

/* Communication */
// WiFi
const char *ssid = "";          // Change this to your WiFi SSID
const char *pass = "";  // Change this to your WiFi password
String serverName = "";
// Ethernet
char server[] = "102.84.207.233";    // name address for Google (using DNS)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/* Millis */
long now = millis(), lastMeasure = 0, timer = 1000000;
int offline = 0;

/* Startup Display */
const unsigned char henafra [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0x80, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3c, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 
	0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x70, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x30, 
	0x00, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x02, 0x07, 0x81, 0xe1, 0xe0, 0xff, 
	0xc0, 0x60, 0x00, 0x00, 0x06, 0x07, 0x81, 0xf3, 0xf8, 0xff, 0xe0, 0x20, 0x00, 0x00, 0x04, 0x07, 
	0x81, 0xf7, 0xf8, 0xff, 0xe0, 0x30, 0x00, 0x00, 0x0c, 0x07, 0x81, 0xff, 0xfc, 0xff, 0xe0, 0x10, 
	0x00, 0x00, 0x08, 0x07, 0x81, 0xff, 0x3c, 0xf0, 0x00, 0x10, 0x00, 0x00, 0x08, 0x07, 0x81, 0xff, 
	0x3c, 0xf0, 0x00, 0x18, 0x00, 0x00, 0x18, 0x07, 0x81, 0xf7, 0xfc, 0xf0, 0x00, 0x08, 0x00, 0x00, 
	0x10, 0x07, 0x81, 0xf7, 0xf8, 0xf0, 0x00, 0x08, 0x00, 0x00, 0x10, 0x07, 0xff, 0xf3, 0xf0, 0xff, 
	0xc0, 0x08, 0x00, 0x00, 0x10, 0x07, 0xff, 0xff, 0xf0, 0xff, 0xe0, 0x0c, 0x00, 0x00, 0x10, 0x07, 
	0xff, 0xff, 0xf1, 0xff, 0xe0, 0x0c, 0x00, 0x00, 0x10, 0x07, 0xff, 0xff, 0xfb, 0xff, 0xe0, 0x04, 
	0x00, 0x00, 0x30, 0x07, 0x81, 0xfe, 0x3f, 0xf0, 0x00, 0x04, 0x00, 0x00, 0x30, 0x07, 0x81, 0xfc, 
	0x3f, 0xf0, 0x00, 0x04, 0x00, 0x00, 0x30, 0x07, 0x81, 0xfc, 0x1f, 0xf0, 0x00, 0x04, 0x00, 0x00, 
	0x30, 0x07, 0x81, 0xfc, 0x3f, 0xf0, 0x00, 0x04, 0x00, 0x00, 0x10, 0x07, 0x81, 0xff, 0xff, 0xf0, 
	0x00, 0x04, 0x00, 0x00, 0x10, 0x07, 0x81, 0xff, 0xff, 0xf0, 0x00, 0x0c, 0x00, 0x00, 0x10, 0x07, 
	0x81, 0xff, 0xfb, 0xf0, 0x00, 0x0c, 0x00, 0x00, 0x10, 0x03, 0x80, 0xf3, 0xe1, 0xf0, 0x00, 0x08, 
	0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 
	0x08, 0x00, 0x32, 0x67, 0xf9, 0xd6, 0x00, 0x10, 0x00, 0x00, 0x0c, 0x00, 0x2a, 0x94, 0xb2, 0x56, 
	0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x2e, 0xbc, 0xba, 0x1e, 0x00, 0x30, 0x00, 0x00, 0x06, 0x00, 
	0x2a, 0x94, 0xb3, 0x56, 0x00, 0x20, 0x00, 0x00, 0x02, 0x00, 0x32, 0x64, 0xb9, 0xd6, 0x00, 0x60, 
	0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 
	0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
	0x02, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 
	0x00, 0x00, 0x00, 0x06, 0x40, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x03, 0x10, 0x00, 
	0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc4, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x71, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3c, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00
};

WiFiClient client;
TimerHandle_t wifiReconnectTimer;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
EthernetClient EClient;

void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  Serial1.begin(38400, SERIAL_8N1, RXD1, TXD1);
  Serial2.begin(38400, SERIAL_8N1, RXD2, TXD2);
  initOLED();
  initTask();
  initEthernet();
  // initWiFiTimer();
  // delay(1000);
  // WiFi.onEvent(WiFiEvent);
  // initWifi();
  delay(3000);
}

void loop() {
  // Restart the service periodically
  now = millis();
  if (now - lastMeasure > timer) 
  {
    lastMeasure = now;
    if(offline == 1)
    {
      ESP.restart();
    }
  }
}
