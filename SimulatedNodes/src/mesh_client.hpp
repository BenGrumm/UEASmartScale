#ifndef MESH_CLIENT_HPP
#define MESH_CLIENT_HPP

#include "IPAddress.h"
#include "painlessMesh.h"
#include "node_simulator.hpp"
#include "io.hpp"

#include <AsyncTCP.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

// Message types
#define FIND_BRIDGE "FIND_BRIDGE"                               // When a node is looking for bridge
#define BRIDGE_KNOWN "BRIDGE_DECLERATION"                       // When the bridge is telling its id
#define UPDATE_SETTINGS "UPDATE_SETTINGS"                       // When a node is updating it's settings
#define RECIEVED_UPDATED_SETTINGS "RECIEVED_UPDATED_SETTINGS"   // When a node is recieving settings updates
#define SERVER_RECIEVED_SETTINGS "SERVER_RECIEVED_SETTINGS"     // When the server acks your updated settings

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

void sendUpdatedSettings(void);
void askForBridge(void);
void newConnectionCallback(uint32_t nodeId);

void updateNumStored(unsigned int numItems);

void addSettingsItemForMeshToSend(String key, String value);
void addSettingsItemForMeshToSend(String key, int value);
void addSettingsItemForMeshToSend(String key, double value);
void addSettingsItemForMeshToSend(String key, unsigned int value);

void clearSettings(void);

/**
 * @brief Function that will iterate through nodes in the mesh and check if current node 
 * has a bridge in the network
 * 
 * @return true if node knows about valid bridge
 * @return false if node knows about no bridge or bridge id not in current network
 */
bool checkIfBridgeExists(void);

void receivedCallback(const uint32_t &from, const String &msg);
void whenConnected(void);

#endif