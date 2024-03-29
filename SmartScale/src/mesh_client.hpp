#ifndef MESH_CLIENT_HPP
#define MESH_CLIENT_HPP

#include "IPAddress.h"
#include "painlessMesh.h"
#include "storage.hpp"

#include <AsyncTCP.h>
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

#define MESH_PORT 6666

#define HOSTNAME "HTTP_BRIDGE"

// Message types
#define FIND_BRIDGE "FIND_BRIDGE"                          // When a node is looking for bridge
#define BRIDGE_KNOWN "BRIDGE_DECLERATION"                  // When the bridge is telling its id
#define UPDATE_SETTINGS "UPDATE_SERVER_SETTINGS"           // When a node is updating it's settings 
#define RECIEVED_UPDATED_SETTINGS "UPDATE_NODE_SETTINGS"   // When a node is recieving settings updates
#define ACK_RECEIVED_SETTINGS "ACK_NODE_RECEIVED"          // When a node received new settings from server and it responds ack
#define SERVER_RECIEVED_SETTINGS "ACK_SERVER_RECEIVED"     // When the server acks your updated settings

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

/**
 * @brief Get the Num Connected Nodes on network
 * 
 * @return int number of nodes
 */
int getNumConnectedNodes(void);

/**
 * @brief update the number of items stored in the buffer object
 * 
 * @param numItems 
 */
void updateNumStored(unsigned int numItems);

/**
 * @brief Functions for updating settings in the buffer object
 * 
 * @param key used to set value in object
 * @param value to set
 */
void addSettingsItemForMeshToSend(String key, String value);
void addSettingsItemForMeshToSend(String key, int value);
void addSettingsItemForMeshToSend(String key, double value);
void addSettingsItemForMeshToSend(String key, unsigned int value);

/**
 * @brief Add a beacon to the updated settings
 * 
 * @param key name of object to create / updated
 * @param major major value of beacon
 * @param minor minor value of beacon
 * @param distance between beacon and device
 */
void addBeacon(String key, uint8_t major, uint8_t minor, double distance);

/**
 * @brief Send ack to node that the server successfully recevied its settings
 * 
 * @param id of the node to send ack to
 */
void rootSendUpdateAck(uint32_t id);

/**
 * @brief Function that will iterate through nodes in the mesh and check if current node 
 * has a bridge in the network
 * 
 * @return true if node knows about valid bridge
 * @return false if node knows about no bridge or bridge id not in current network
 */
bool checkIfBridgeExists(void);

/**
 * @brief Send updated settings received from server to node
 * 
 * @param settings json object (must have "id" value)
 */
void sendSettingsToNode(JsonObject settings);

/**
 * @brief Get the mesh id
 * 
 * @return uint32_t id number
 */
uint32_t getMeshID(void);

/**
 * @brief check if the bridge has been found and successfully sent to
 * 
 * @return true if bridge found
 * @return false if bridge not yet been found / used
 */
bool getIfBridgeExists(void);

// Internal functions

void sendUpdatedSettings(void);
void askForBridge(void);
void ackUpdatedSettings(void);
void clearSettings(void);
bool checkIfNodeInNetwork(uint32_t nodeID);
void receivedCallback(const uint32_t &from, const String &msg);

#endif