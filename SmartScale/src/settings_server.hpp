#ifndef SETTINGS_SERVER_HPP
#define SETTINGS_SERVER_HPP

#include "IPAddress.h"
#include "painlessMesh.h"

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define AP_SERVER_PORT 80

/**
 * @brief Function for setting up the http server that will be hosted on the esp
 * 
 * @param meshAPIP IP adress that is given by the mesh
 */
void setupServer(IPAddress meshAPIP);

/**
 * @brief Function to turn the http server on
 * 
 */
void httpServerOn(void);

/**
 * @brief Function to stop hosting the http server
 * 
 */
void httpServerOff(void);

/**
 * @brief Function to get a string of the ip and port the http server is on so you can connect
 * 
 * @return String of the full ip and port number
 */
String getAPIPStr(void);

#endif