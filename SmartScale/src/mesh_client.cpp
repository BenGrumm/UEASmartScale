#include "mesh_client.hpp"

painlessMesh  mesh;
IPAddress myIP(0,0,0,0);

DynamicJsonDocument updatedSettings(2048);
#ifdef ROOT
DynamicJsonDocument sentNodeSettings(1024);
#endif
bool hasUpdatedSinceLastSend = false;
bool bridgeExists = false;

JsonObject updatedSettingsObject;

Task periodicSettingsUpdates(TASK_SECOND * 30, TASK_FOREVER, &sendUpdatedSettings);

unsigned int lastNumItemsSent = 941; // Random initializer as value likely to be 0 at setup

/**
 * @brief Function to set up the devices mesh client
 * 
 * @param userScheduler the scheduler that the mesh will update on every loop
 */
void setupMesh(Scheduler &userScheduler){
    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

    // Channel set to 6. Make sure to use the same channel for your mesh and for you other
    // network (STATION_SSID)
    mesh.init("MESH_" + deviceSettings.meshName, "PASS_" + deviceSettings.meshPassword, &userScheduler, MESH_PORT, WIFI_AP_STA, 9);
    // mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6);
    mesh.onReceive(&receivedCallback);

    #ifdef ROOT

    Serial.println("Setup Mesh WIFI");

    // To connect to wifi
    // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
    Serial.println("SSID \"" + deviceSettings.WIFISSID + "\", Password = \"" + deviceSettings.WIFIPassword + "\"");
    mesh.stationManual(deviceSettings.WIFISSID, deviceSettings.WIFIPassword);
    mesh.setHostname(HOSTNAME);
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    mesh.setRoot(true);
    // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
    mesh.setContainsRoot(true);

    #endif

    updatedSettingsObject = updatedSettings.createNestedObject(UPDATE_SETTINGS);

    // If first startup send initial settings
    if(deviceSettings.initialisation){
        addSettingsItemForMeshToSend(NUM_ITEMS_PER_WEIGHT_KEY, deviceSettings.numItemsPerWeight);
        addSettingsItemForMeshToSend(WEIGHT_PER_X_KEY, deviceSettings.referenceWeight);
        // TODO Set initialisation to false
    }

    userScheduler.addTask(periodicSettingsUpdates);
    periodicSettingsUpdates.enableDelayed(TASK_SECOND * 15);

}

/**
 * @brief Get the Mesh APIP object
 * 
 * @return IPAddress that is used by the mesh
 */
IPAddress getMeshAPIP(void){
    return IPAddress(mesh.getAPIP());
}

/**
 * @brief Callback used by the mesh when it recieves a message
 * 
 * @param from id of the message sender
 * @param msg message from the sender
 */
void receivedCallback(const uint32_t &from, const String &msg){
    Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
    DynamicJsonDocument jsonBuffer(1024 + msg.length());
    deserializeJson(jsonBuffer, msg);
    JsonObject root = jsonBuffer.as<JsonObject>();

    // If is the root and receives a message asking to find bridge broadcast the
    // id of this bridge
    if(root.containsKey(FIND_BRIDGE) && mesh.isRoot()){
        Serial.println("Send BRIDGE");
        String outputMsg;
        StaticJsonDocument<1024> jsonBuffer;
        JsonObject obj = jsonBuffer.createNestedObject();
        obj[BRIDGE_KNOWN] = mesh.getNodeId();
        serializeJson(obj, outputMsg);
        
        mesh.sendBroadcast(outputMsg, true);
    }
    // If is root will store settings from all connected nodes and periodically upload them
    // when it receives an object names UPDATE_SETTINGS will add to list that will update
    else if(root.containsKey(UPDATE_SETTINGS) && mesh.isRoot()){
        #ifdef ROOT
        Serial.println("Updating settings");
        // Bridge has recieved a nodes updated settings add to list
        JsonObject obj = root[UPDATE_SETTINGS];
        addUpdatedSettings(obj);
        #endif
    }
    // Received a message from bridge on network with its id
    else if(root.containsKey(BRIDGE_KNOWN)){
        // Recieved the id of bridge on the current network
        if(deviceSettings.bridgeID != root[BRIDGE_KNOWN]){
            setBridgeID(root[BRIDGE_KNOWN]);
        }
    }
    // Received a message giving updated settings
    else if(root.containsKey(RECIEVED_UPDATED_SETTINGS)){
        // TODO Read update settings locally
        updateLocalSettings(root[RECIEVED_UPDATED_SETTINGS]);
        // Return ack
        ackUpdatedSettings();
    }
    // The settings sent to root from this node successfully updated on the server
    else if(root.containsKey(SERVER_RECIEVED_SETTINGS)){
        clearSettings();
    }
    // Root receives this if node successfully received its settings update
    else if(root.containsKey(ACK_RECEIVED_SETTINGS) && mesh.isRoot()){
        #ifdef ROOT
        addSettingsAckID(from);
        #endif
    }
}

/**
 * @brief when the settings are sent to root then server and ack received clear
 * the updates 
 * 
 */
void clearSettings(void){
    Serial.println("Clearing settings");
    if(!hasUpdatedSinceLastSend){
        Serial.println("Have Cleared");
        updatedSettingsObject.clear();
    }
}

/**
 * @brief Send an ack notifying received settings to root
 * 
 */
void ackUpdatedSettings(void){
    if(mesh.isRoot()){
        Serial.println("Is Root Update Ack");
        #ifdef ROOT
        addSettingsAckID(mesh.getNodeId());
        #endif
    }else{
        mesh.sendSingle(deviceSettings.bridgeID, "{\"ACK_RECEIVED\":true");
    }
}

/**
 * @brief Send updated settings received from server to node
 * 
 * @param settings json object (must have "id" value)
 */
void sendSettingsToNode(JsonObject settings){
    Serial.print("Sending updated settings to node ");
    Serial.println((uint32_t)settings["id"]);

    #ifdef ROOT

    if(((uint32_t)settings["id"]) == mesh.getNodeId()){
        Serial.println("ID is me :)");
        updateLocalSettings(settings);
        ackUpdatedSettings();
        return;
    }

    sentNodeSettings.clear();
    JsonObject obj = sentNodeSettings.createNestedObject(RECIEVED_UPDATED_SETTINGS);
    obj.set(settings);

    String msg;

    serializeJson(sentNodeSettings, msg);
    Serial.println(msg);

    mesh.sendSingle((uint32_t)settings["id"], msg);

    #endif
}

/**
 * @brief Send updated settings stored in buffer to root if exists
 * 
 */
void sendUpdatedSettings(void){
    if(updatedSettingsObject.size() > 0 && checkIfBridgeExists()){
        Serial.println("Is Settings To Update And Bridge Exists");
        serializeJsonPretty(updatedSettings, Serial); Serial.println();
        updatedSettingsObject["id"] = mesh.getNodeId();
        
        if(deviceSettings.bridgeID != mesh.getNodeId()){
            String msg;
            serializeJson(updatedSettings, msg);
            Serial.println(msg);
        
            Serial.println("Not Bridge So Send to bridge");
            mesh.sendSingle(deviceSettings.bridgeID, msg);
        }
        #ifdef ROOT
        else{
            Serial.println("Am Bridge");
            addUpdatedSettings(updatedSettingsObject);
        }
        #endif

        hasUpdatedSinceLastSend = false;
    }
}

/**
 * @brief Send ack to node that the server successfully recevied its settings
 * 
 * @param id of the node to send ack to
 */
void rootSendUpdateAck(uint32_t id){
    if(id == mesh.getNodeId()){
        clearSettings();
    }else{
        mesh.sendSingle(id, "{\"ACK_SERVER_RECEIVED\":true}");
    }
}

/**
 * @brief Check if the bridge exists on current network
 * 
 * @return true if bridge found
 * @return false if bridge not found
 */
bool checkIfBridgeExists(void){
    if(deviceSettings.bridgeID != 0){
        if(checkIfNodeInNetwork(deviceSettings.bridgeID)){
            Serial.print("Checked And Found Bridge ID In List - ");
            Serial.println(deviceSettings.bridgeID);
            bridgeExists = true;
            return true;
        }
    }

    askForBridge();

    bridgeExists = false;

    return false;
}

/**
 * @brief Function to check if a node exists on network
 * 
 * @param nodeID id of node to check on network
 * @return true if node found
 * @return false if node not found
 */
bool checkIfNodeInNetwork(uint32_t nodeID){
    SimpleList<uint32_t> nl = mesh.getNodeList(true);
    SimpleList<uint32_t>::iterator itr = nl.begin();

    while(itr != nl.end()) {
        if(*itr == nodeID){
            return true;
        }
        itr++;
    }

    return false;
}

/**
 * @brief Get the mesh id
 * 
 * @return uint32_t id number
 */
uint32_t getMeshID(void){
    return mesh.getNodeId();
}

/**
 * @brief check if the bridge has been found and successfully sent to
 * 
 * @return true if bridge found
 * @return false if bridge not yet been found / used
 */
bool getIfBridgeExists(void){
    return bridgeExists;
}

/**
 * @brief Get the Num Connected Nodes on network
 * 
 * @return int number of nodes
 */
int getNumConnectedNodes(void){
    return mesh.getNodeList(true).size();
}

/**
 * @brief update the number of items stored in the buffer object
 * 
 * @param numItems 
 */
void updateNumStored(unsigned int numItems){
    updatedSettingsObject[NUM_STORED_KEY] = numItems;
    hasUpdatedSinceLastSend = true;
}

/**
 * @brief Functions for updating settings in the buffer object
 * 
 * @param key used to set value in object
 * @param value to set
 */
void addSettingsItemForMeshToSend(String key, String value){
    updatedSettingsObject[key] = value;
    hasUpdatedSinceLastSend = true;
}
void addSettingsItemForMeshToSend(String key, int value){
    updatedSettingsObject[key] = value;
    hasUpdatedSinceLastSend = true;
}
void addSettingsItemForMeshToSend(String key, double value){
    updatedSettingsObject[key] = value;
    hasUpdatedSinceLastSend = true;
}
void addSettingsItemForMeshToSend(String key, unsigned int value){
    updatedSettingsObject[key] = value;
    hasUpdatedSinceLastSend = true;
}

/**
 * @brief Add a beacon to the updated settings
 * 
 * @param key name of object to create / updated
 * @param major major value of beacon
 * @param minor minor value of beacon
 * @param distance between beacon and device
 */
void addBeacon(String key, uint8_t major, uint8_t minor, double distance){

    JsonObject obj;

    if(updatedSettingsObject.containsKey(key)){
        obj = updatedSettingsObject[key];
    }else{
        obj = updatedSettingsObject.createNestedObject(key);
    }

    obj["major"] = major;
    obj["minor"] = minor;
    obj["distance"] = distance;
}

/**
 * @brief Broadcast to the network the need to find bridge
 * 
 */
void askForBridge(void){
    // TODO replace this with proper enum
    mesh.sendBroadcast("{ \"FIND_BRIDGE\" : true }", true);
}

/**
 * @brief Function to update mesh in main loop
 * 
 */
void loopMesh(){
    mesh.update();
}