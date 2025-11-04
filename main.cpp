/*
 * ----------------------------------------------
 * Project/Program Name : tilt and button upload data
 * File Name            : <dataupload.cpp>
 * Author               : Isaac Rodriguez
 * Date                 : Nov 3, 2025
 * Version              : V1
 * 
 * Purpose:
 *   This program uses a ESP8266, 1 temperature/humidity sensor , 1 tilt switch and 1 button.
 *    This program connects to the internet and asks the user to verify the timezone. Then 
 *    it waits for the tilt switch or button to be actuated. Then it sends data to an online database 
 *    where I have a dashboard.
 * 
 * Inputs:
 *   1 tilt switch, 1 button and 1 temperature/humidity sensor.
 * 
 * Outputs:
 *    WiFi signal to database
 * 
 * Example Application:
 *    In this application. This program is used as a driver for the different functions that run this application.
 * 
 * Dependencies:
 *    espIoT.h
 * 
 * ---------------------------------------------------------------------------
 */



#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "espIoT.h"

const char* ssid     = "dddddddddd"; 
const char* password = "dddddddddd";

// const char* ssid     = "ddddddddddd"; 
// const char* password = "ddddddddddd";

String baseURL = "https://isaacr.org/db_insertV2.php?";
String nodeURL = "nodeId=node-5";
String timeURL = "https://timeapi.io/api/time/current/zone?timeZone=";
String dataURL;

int tilt = 5;
int button = 4;
int sensor = 16;

// DHTesp dht;
EspIoT UI;
EspIoT _8266;

void setup() {
  _8266.setup(tilt, button, sensor); // setup pinmodes for switches, tilt(5), button(4), sensor(16)

  Serial.begin(9600);

  String thisBoard= ARDUINO_BOARD;
  // setup temp and humidity sensor

  WiFi.mode(WIFI_STA); // SETS TO STATION MODE!
  //WiFi.disconnect();
  Serial.println("");
  WiFi.hostname("ESP-host");
  delay(100);
  Serial.println("Connecting to WiFi");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".+.");
  }
  Serial.println("");

  Serial.println(WiFi.getHostname());
  Serial.println("UI time select");
  UI.timeSelect_message();

  timeURL = _8266.read_userInput(Serial.parseInt(), timeURL); // Read until a newline character
}
void loop() { 
  if (WiFi.status() == WL_CONNECTED)
  {
    bool ready = false;

    while (ready == false)
    {
      nodeURL = _8266.check_switches(tilt, button);
      Serial.print(".-.");
      if (nodeURL != "")
        {
          ready = true;
        }
    }
  Serial.println("");
  dataURL = _8266.read_time(baseURL, timeURL, nodeURL);

  if(_8266.error_check(dataURL) == true)
  {
    dataURL = _8266.read_humidity(dataURL);
    dataURL = _8266.read_temperature(dataURL);
    _8266.transmit(dataURL);
  }
  else
  {
    Serial.println("No datetime data");
  }
  delay(5000);
}}
