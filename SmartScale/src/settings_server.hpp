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

void setupServer(IPAddress meshAPIP);
void httpServerOn(void);
void httpServerOff(void);
String getAPIPStr(void);

#endif