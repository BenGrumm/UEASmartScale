#ifndef HTTP_REQUESTS_H
#define HTTP_REQUESTS_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "painlessMesh.h"
#include "storage.hpp"
#include "mesh_client.hpp"

const String SERVER_IP = "http://192.168.1.101:8000";

/**
 * @brief Setup json objects used in http_request functions. Create recurrent task on 
 * other core for making requqests.
 * 
 * @param userScheduler * was switched to rtos function so no longer needed
 */
void setupHTTP(Scheduler &userScheduler);

/**
 * @brief Add all settings from JsonObject to buffer of objects to be uploaded
 * to server in http request
 * 
 * @param scaleSettings Object containing id of node and rest of settings to update
 */
void addUpdatedSettings(JsonObject scaleSettings);

/**
 * @brief Get if the username and password have successfully made a request so user
 * knows if there is a problem
 * 
 * @return true is successful request
 * @return false if either no request was made yet or requests havn't been successful
 */
bool hadSuccessfulLogin(void);

// Function used in file only
bool authorise(void);
void uploadSettings(void* args);
bool updateSettings(void);
bool getSettingsToUpdate(void);
void addSettingsAckID(uint32_t id);

#endif