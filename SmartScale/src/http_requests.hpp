#ifndef HTTP_REQUESTS_H
#define HTTP_REQUESTS_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "painlessMesh.h"
#include "storage.hpp"


void setupHTTP(Scheduler &userScheduler);
bool authorise(void);
void tryConnect(void* args);

#endif