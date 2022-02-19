#ifndef HTTP_REQUESTS_H
#define HTTP_REQUESTS_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "painlessMesh.h"

const String SERVER_IP = "http://0.0.0.0:8000";

void setupHTTP(Scheduler &userScheduler);
bool authorise(void);
void tryConnect(void* args);

#endif