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
#include "storage.hpp"
#include "http_requests.hpp"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define   STATION_SSID     "TP-LINK_947990"
#define   STATION_PASSWORD "44020712"

#define HOSTNAME "HTTP_BRIDGE"

// Message types
#define FIND_BRIDGE "FIND_BRIDGE"                               // When a node is looking for bridge
#define BRIDGE_KNOWN "BRIDGE_DECLERATION"                       // When the bridge is telling its id
#define UPDATE_SETTINGS "UPDATE_SETTINGS"                       // When a node is updating it's settings
#define RECIEVED_UPDATED_SETTINGS "RECIEVED_UPDATED_SETTINGS"   // When a node is recieving settings updates
#define UPDATE_NUM_ITEMS "UPDATE_ITEMS"                         // When a node is updating the number of items

#define NUM_STORED_KEY "numStored"
#define WEIGHT_PER_X_KEY "weightGramsPerXItmes"
#define NUM_ITEMS_PER_WEIGHT_KEY "numItemsPerWeight"
#define MIN_NUM_ITEMS_KEY "minNumItems"
#define OK_NUM_ITEMS_KEY "okNumItems"

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

void sendNumStored(void);
void sendUpdatedSettings(void);
void askForBridge(void);

void updateNumStored(unsigned int numItems);

void addSettingsItemForMeshToSend(String key, String value);
void addSettingsItemForMeshToSend(String key, int value);
void addSettingsItemForMeshToSend(String key, double value);
void addSettingsItemForMeshToSend(String key, unsigned int value);

/**
 * @brief Function that will iterate through nodes in the mesh and check if current node 
 * has a bridge in the network
 * 
 * @return true if node knows about valid bridge
 * @return false if node knows about no bridge or bridge id not in current network
 */
bool checkIfBridgeExists(void);

void receivedCallback(const uint32_t &from, const String &msg);

#endif