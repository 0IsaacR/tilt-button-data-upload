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
 *    In this application. This program is used as a library for all our functions. It dictates exactly what our functions do.
 * 
 * Dependencies:
 *    espIoT.h
 * 
 * ---------------------------------------------------------------------------
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "espIoT.h"
#include "DHTesp.h"
// #include <iostream>
// EspIoT::EspIoT(int pin) : _pin(0)
DHTesp dht;

void EspIoT::setup(int tiltPin, int switchPin, int sensorPin)
{
    pinMode(tiltPin, INPUT);
    pinMode(switchPin, INPUT);
    dht.setup(sensorPin, DHTesp::DHT22);
}


// User Interface message: choose timezone for data to be time stamp with current time based on time zone.
void EspIoT::timeSelect_message(void)
{
    Serial.println("");
    // time zone choices
    Serial.println("—> Select Your Time Zone (The default time is PT):  ");
    Serial.println("1)	Eastern Time Zone (ET) - New York City, NY");
    Serial.println("2)	Central Time Zone (CT) - Chicago, IL");
    Serial.println("3)	Mountain Time Zone (MT) - Denver, CO");
    Serial.println("4)	Pacific Time Zone (PT) - Los Angeles, CA");
    Serial.println("5)	Alaska Time Zone (AKT) - Anchorage, AK");
    Serial.println("6)	Hawaii-Aleutian Time Zone (HAT) - Honolulu, HI");
    Serial.println("7)	Atlantic Time Zone (AT) - San Juan, Puerto Rico (US territory");
    Serial.println("");

    while (Serial.available() == 0) 
      {
       // Do nothing, just wait for serial data / user input
      }
}

String  EspIoT::read_userInput(int userInput, String timeURL)
{
    String timeZone = "";
    String dateTime = "";
    // Clear the input buffer of any remaining characters (like newline)
    while (Serial.available()) 
      {
        Serial.read();
      }
    switch (userInput) {
        case 1:
            timeZone = _ET;
            break;
        case 2:
            timeZone = _CT;
            break;
        case 3:
            timeZone = _MT;
            break;
        case 4:
            timeZone = _PT;
            break;
        case 5:
            timeZone = _AKT;
            break;
        case 6:
            timeZone = _HAT;
            break;
        case 7:
            timeZone = _AT;
            break;
        default:
            timeZone = _PT;
            break;
    }
  Serial.println("Your timezone is set to: " + timeZone);
  timeURL += timeZone;       // saved as private to later be used
  /***********************  node data is hard coded   ************************/
  // nodeURL = "nodeId=node-5";
  return timeURL;
}


String EspIoT::check_switches(int tilt, int button)
    {
        String nodeURL = "";
        if (digitalRead(tilt) == LOW)
            {
                nodeURL = "nodeId=node-1";
                // return nodeURL;
            }
        if (digitalRead(button) == LOW)
            {
                nodeURL = "nodeId=node-2";
                // return nodeURL;
            }
        delay(500);
        return nodeURL;
    }

String EspIoT::read_time(String baseURL, String timeURL, String nodeURL)
{ 
     String dateTime;
    // _baseURL = "https://isaacr.org/db_insertV2.php?";
    String dataURL = baseURL;
    dataURL += nodeURL;
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;
    // READ TIME
    Serial.println("Requesting Time: -->" + timeURL);
    if (https.begin(client, timeURL)){
     
      int httpCode = https.GET();
      delay(750);
      // int httpCode = https.POST(fullUrl); // for post comms

      Serial.println("Response code <--: " + String(httpCode));
      if (httpCode > 0)
        {
          dateTime = https.getString();

          StaticJsonDocument<200> doc;
          DeserializationError error = deserializeJson(doc, dateTime);
          if (error) 
            {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
            }
          String hour = doc["hour"];
          String minute = doc["minute"];
          String second = doc["seconds"];
          String day = doc["day"];
          String month = doc["month"];
          String year = doc["year"];
          if (second.length() < 2){
            second = "0"+second;
          }
          if (minute.length() < 2){
            minute = "0"+minute;
          }
          if (hour.length() < 2){
            hour = "0"+hour;
          }
          if (day.length() < 2){
            day = "0"+day;
          } 
          if (month.length() < 2){
            month = "0"+month;
          }        
          Serial.println("");
          Serial.println("today is: " + month + "/" + day + "/" + year);
          Serial.println("the time is: " + hour + ":" + minute + ":" + second);
          String db_datetime = year + "-" + month + "-" + day + "+" + hour + "%3A" + minute + "%3A" + second;
          //Serial.println("database DT: " + db_datetime);
          Serial.println("");
          
          String timeData = "&timeReceived=" + db_datetime;

          dataURL += timeData; 
          
        }
      else
        {
          dataURL = "0";
        }
      https.end();
    }   

    return dataURL;
}

String EspIoT::read_humidity(String dataURL)
    {   
        String humData = "&nodeHum=";
        delay(dht.getMinimumSamplingPeriod());
        String humidity = String(dht.getHumidity()/35);
        dataURL += humData;
        dataURL += humidity;
        return dataURL;
    }

String EspIoT::read_temperature(String dataURL)
    {
        String tempData = "&nodeTemp=";
        delay(dht.getMinimumSamplingPeriod());
        String temperature = String(dht.getTemperature()/9);
        dataURL += tempData;
        dataURL += temperature;
        return dataURL;
    }

void EspIoT::transmit(String dataURL) // Transmit to Database
    {
        WiFiClientSecure client;
        client.setInsecure();
        HTTPClient https;
        Serial.println("Sending to dB: -->" + dataURL);

        if (https.begin(client, dataURL))
        {
            int httpCode = https.GET();
            delay(1000);
            // int httpCode = https.POST(fullUrl); // for post comms
            if (httpCode > 0)
            {
                //Serial.println(https.getString());
                Serial.println("Data sent");
            }
            https.end();
        }
        else
        {
            Serial.println("[HTTPS] Unable to connect");
        }
        dataURL = "";
    }

bool EspIoT::error_check(String dataURL)
     {
        if (dataURL == "0")
        {
          return false;
        }
        else
        {
          return true;
        }

     }
