#ifndef MESH_CLIENT_HPP
#define MESH_CLIENT_HPP

#include "IPAddress.h"
#include "painlessMesh.h"
#include "storage.hpp"

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

#ifdef ROOT
#include "http_requests.hpp"
#endif

#define MESH_NAME_SIZE 5
#define MESH_PASSWORD_SIZE 5

#define   MESH_PORT       6666

#define HOSTNAME "HTTP_BRIDGE"

// Message types
#define FIND_BRIDGE "FIND_BRIDGE"                               // When a node is looking for bridge
#define BRIDGE_KNOWN "BRIDGE_DECLERATION"                       // When the bridge is telling its id
#define UPDATE_SETTINGS "UPDATE_SETTINGS"                       // When a node is updating it's settings
#define RECIEVED_UPDATED_SETTINGS "RECIEVED_UPDATED_SETTINGS"   // When a node is recieving settings updates
#define ACK_RECEIVED_SETTINGS "ACK_RECEIVED"                    // When a node received new settings from server and it responds ack
#define SERVER_RECIEVED_SETTINGS "SERVER_RECIEVED_SETTINGS"     // When the server acks your updated settings

#define NUM_STORED_KEY "numStored"
#define WEIGHT_PER_X_KEY "weightGramsPerXItmes"
#define NUM_ITEMS_PER_WEIGHT_KEY "numItemsPerWeight"
#define MIN_NUM_ITEMS_KEY "minNumItems"
#define OK_NUM_ITEMS_KEY "okNumItems"
#define UPDATE_SETTINGS_SERVER "updateSettings"

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

void sendUpdatedSettings(void);
void askForBridge(void);
int getNumConnectedNodes(void);

void ackUpdatedSettings(void);

void updateNumStored(unsigned int numItems);

void addSettingsItemForMeshToSend(String key, String value);
void addSettingsItemForMeshToSend(String key, int value);
void addSettingsItemForMeshToSend(String key, double value);
void addSettingsItemForMeshToSend(String key, unsigned int value);

void rootSendUpdateAck(uint32_t id);
void clearSettings(void);

/**
 * @brief Function that will iterate through nodes in the mesh and check if current node 
 * has a bridge in the network
 * 
 * @return true if node knows about valid bridge
 * @return false if node knows about no bridge or bridge id not in current network
 */
bool checkIfBridgeExists(void);
bool checkIfNodeInNetwork(uint32_t nodeID);
void sendSettingsToNode(JsonObject settings);
uint32_t getMeshID(void);

bool getIfBridgeExists(void);

void receivedCallback(const uint32_t &from, const String &msg);

#endif