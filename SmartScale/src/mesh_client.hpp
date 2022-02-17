#ifndef MESH_CLIENT_HPP
#define MESH_CLIENT_HPP

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

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define   STATION_SSID     "TP-LINK_947990"
#define   STATION_PASSWORD "44020712"

#define HOSTNAME "HTTP_BRIDGE"

/**
 * @brief Function to set up the devices mesh client
 * 
 * @param userScheduler the scheduler that the mesh will update on every loop
 */
void setupMesh(Scheduler &userScheduler);

/**
 * @brief Get the Mesh APIP object
 * 
 * @return IPAddress that is used by the mesh
 */
IPAddress getMeshAPIP(void);

/**
 * @brief Function to call in the arduino loop method
 * 
 */
void loopMesh(void);

void receivedCallback(const uint32_t &from, const String &msg);

#endif