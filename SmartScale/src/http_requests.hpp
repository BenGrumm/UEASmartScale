#ifndef HTTP_REQUESTS_H
#define HTTP_REQUESTS_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "painlessMesh.h"
#include "storage.hpp"
#include "mesh_client.hpp"

const String SERVER_IP = "http://192.168.1.101:8000";

void setupHTTP(Scheduler &userScheduler);
bool authorise(void);
void uploadSettings(void* args);
void addUpdatedSettings(JsonObject scaleSettings);
bool updateSettings(void);
bool hadSuccessfulLogin(void);
bool getSettingsToUpdate(void);
void addSettingsAckID(uint32_t id);

#endif