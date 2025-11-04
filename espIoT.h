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
 *    In this application. This program is used as a declaration page where we state the inputs and outputs of all our functions.
 * 
 * ---------------------------------------------------------------------------
 */



#ifndef ESPIOT_H
#define ESPIOT_H

class EspIoT
{
    int tilt;
    int button;
    int sensor;
    
public:
    EspIoT(void) = default; 
    String read_userInput(int userInput, String timeURL);
    void setup(int tilt, int button, int sensor);
    void timeSelect_message(void);
    String check_switches(int tiltPin, int switchPin);
    String read_time(String baseURL, String timeURL, String nodeURL);
    void transmit(String dataURL);
    String read_temperature(String dataURL);
    String read_humidity(String dataURL);
    bool error_check(String dataURL);


private:

    
    // Location data
    String _ET = "America/New_York";
    String _CT = "America/Chicago";
    String _MT = "America/Denver";
    String _PT = "America/Los_Angeles";
    String _AKT = "America/Anchorage";
    String _HAT = "Pacific/Honolulu";
    String _AT = "America/Puerto_Rico";
};
#endif
