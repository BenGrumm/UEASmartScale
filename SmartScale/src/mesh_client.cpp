#include "mesh_client.hpp"

painlessMesh  Mesh_Client::mesh;
IPAddress Mesh_Client::myIP(0,0,0,0);
DeviceSettings* Mesh_Client::local_mesh_settings;

DynamicJsonDocument Mesh_Client::updatedSettings(4096);
#ifdef ROOT
DynamicJsonDocument Mesh_Client::sentNodeSettings(2048);
#endif
bool Mesh_Client::hasUpdatedSinceLastSend = false;
bool Mesh_Client::bridgeExists = false;

const String Mesh_Client::beaconKeys[] = {"beaconOne", "beaconTwo", "beaconThree", "beaconFour"};

JsonObject Mesh_Client::updatedSettingsObject;

Task Mesh_Client::periodicSettingsUpdates(TASK_SECOND * 30, TASK_FOREVER, &sendUpdatedSettings);

unsigned int Mesh_Client::lastNumItemsSent = 941; // Random initializer as value likely to be 0 at setup

/**
 * @brief Function to set up the devices mesh client
 * 
 * @param userScheduler the scheduler that the mesh will update on every loop
 */
void Mesh_Client::setupMesh(Scheduler &userScheduler){
    local_mesh_settings = DeviceSettings::getInstance();

    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

    // Channel set to 6. Make sure to use the same channel for your mesh and for you other
    // network (STATION_SSID)
    mesh.init("MESH_" + local_mesh_settings->meshName, "PASS_" + local_mesh_settings->meshPassword, &userScheduler, MESH_PORT, WIFI_AP_STA, 9);
    // mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6);
    mesh.onReceive(&receivedCallback);

    #ifdef ROOT

    Serial.println("Setup Mesh WIFI");

    // To connect to wifi
    // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
    Serial.println("SSID \"" + local_mesh_settings->WIFISSID + "\", Password = \"" + local_mesh_settings->WIFIPassword + "\"");
    mesh.stationManual(local_mesh_settings->WIFISSID, local_mesh_settings->WIFIPassword);
    mesh.setHostname(HOSTNAME);
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    mesh.setRoot(true);

    #endif

    // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
    mesh.setContainsRoot(true);

    updatedSettingsObject = updatedSettings.createNestedObject(UPDATE_SETTINGS);

    // If first startup send initial settings
    if(local_mesh_settings->initialisation){
        addSettingsItemForMeshToSend(NUM_ITEMS_PER_WEIGHT_KEY, local_mesh_settings->numItemsPerWeight);
        addSettingsItemForMeshToSend(WEIGHT_PER_X_KEY, local_mesh_settings->referenceWeight);
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
IPAddress Mesh_Client::getMeshAPIP(void){
    return IPAddress(mesh.getAPIP());
}

/**
 * @brief Callback used by the mesh when it recieves a message
 * 
 * @param from id of the message sender
 * @param msg message from the sender
 */
void Mesh_Client::receivedCallback(const uint32_t &from, const String &msg){
    Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
    DynamicJsonDocument jsonBuffer(2048 + msg.length());
    deserializeJson(jsonBuffer, msg);
    JsonObject root = jsonBuffer.as<JsonObject>();
    if(!root.isNull() && root.size() > 0){
        serializeJsonPretty(root, Serial);
    }else{
        Serial.println("Error Parsing MSG stack issue? TODO look at dynamic json document");
        ESP.restart();
    }

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
        HTTP_Requests::addUpdatedSettings(obj);
        #endif
    }
    // Received a message from bridge on network with its id
    else if(root.containsKey(BRIDGE_KNOWN)){
        // Recieved the id of bridge on the current network
        if(local_mesh_settings->bridgeID != root[BRIDGE_KNOWN]){
            local_mesh_settings->setBridgeID(root[BRIDGE_KNOWN]);
        }
    }
    // Received a message giving updated settings
    else if(root.containsKey(RECIEVED_UPDATED_SETTINGS)){
        // TODO Read update settings locally
        local_mesh_settings->updateLocalSettings(root[RECIEVED_UPDATED_SETTINGS]);
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
        HTTP_Requests::addSettingsAckID(from);
        #endif
    }
}

/**
 * @brief when the settings are sent to root then server and ack received clear
 * the updates 
 * 
 */
void Mesh_Client::clearSettings(void){
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
void Mesh_Client::ackUpdatedSettings(void){
    if(mesh.isRoot()){
        Serial.println("Is Root Update Ack");
        #ifdef ROOT
        HTTP_Requests::addSettingsAckID(mesh.getNodeId());
        #endif
    }else{
        mesh.sendSingle(local_mesh_settings->bridgeID, "{\"ACK_NODE_RECEIVED\":true}");
    }
}

/**
 * @brief Send updated settings received from server to node
 * 
 * @param jsonSettings json object (must have "id" value)
 */
void Mesh_Client::sendSettingsToNode(JsonObject jsonSettings){
    Serial.print("Sending updated settings to node ");
    Serial.println((uint32_t)jsonSettings[SCALE_ID_KEY]);

    #ifdef ROOT

    if(((uint32_t)jsonSettings[SCALE_ID_KEY]) == mesh.getNodeId()){
        Serial.println("ID is me :)");
        local_mesh_settings->updateLocalSettings(jsonSettings);
        ackUpdatedSettings();
        return;
    }

    sentNodeSettings.clear();
    JsonObject obj = sentNodeSettings.createNestedObject(RECIEVED_UPDATED_SETTINGS);
    obj.set(jsonSettings);

    String msg;

    serializeJson(sentNodeSettings, msg);
    Serial.println(msg);

    mesh.sendSingle((uint32_t)jsonSettings[SCALE_ID_KEY], msg);

    #endif
}

/**
 * @brief Send updated settings stored in buffer to root if exists
 * 
 */
void Mesh_Client::sendUpdatedSettings(void){
    if(updatedSettingsObject.size() > 0 && checkIfBridgeExists()){

        Serial.println("Is Settings To Update And Bridge Exists");
        serializeJsonPretty(updatedSettings, Serial); Serial.println();
        updatedSettingsObject[SCALE_ID_KEY] = mesh.getNodeId();
        
        if(local_mesh_settings->bridgeID != mesh.getNodeId() && updatedSettingsObject.containsKey(SCALE_ID_KEY)){
            String msg;
            serializeJson(updatedSettings, msg);
            Serial.println(msg);
        
            Serial.println("Not Bridge So Send to bridge");
            mesh.sendSingle(local_mesh_settings->bridgeID, msg);
        }
        #ifdef ROOT
        else if(local_mesh_settings->bridgeID == mesh.getNodeId()){
            Serial.println("Am Bridge");
            HTTP_Requests::addUpdatedSettings(updatedSettingsObject);
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
void Mesh_Client::rootSendUpdateAck(uint32_t id){
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
bool Mesh_Client::checkIfBridgeExists(void){
    if(local_mesh_settings->bridgeID != 0){
        if(checkIfNodeInNetwork(local_mesh_settings->bridgeID)){
            Serial.print("Checked And Found Bridge ID In List - ");
            Serial.println(local_mesh_settings->bridgeID);
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
bool Mesh_Client::checkIfNodeInNetwork(uint32_t nodeID){
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
uint32_t Mesh_Client::getMeshID(void){
    return mesh.getNodeId();
}

/**
 * @brief check if the bridge has been found and successfully sent to
 * 
 * @return true if bridge found
 * @return false if bridge not yet been found / used
 */
bool Mesh_Client::getIfBridgeExists(void){
    return bridgeExists;
}

/**
 * @brief Get the Num Connected Nodes on network
 * 
 * @return int number of nodes
 */
int Mesh_Client::getNumConnectedNodes(void){
    return mesh.getNodeList(true).size();
}

/**
 * @brief update the number of items stored in the buffer object
 * 
 * @param numItems 
 */
void Mesh_Client::updateNumStored(unsigned int numItems){
    updatedSettingsObject[NUM_STORED_KEY] = numItems;
    hasUpdatedSinceLastSend = true;
}

/**
 * @brief Functions for updating settings in the buffer object
 * 
 * @param key used to set value in object
 * @param value to set
 */
void Mesh_Client::addSettingsItemForMeshToSend(String key, String value){
    updatedSettingsObject[key] = value;
    hasUpdatedSinceLastSend = true;
}
void Mesh_Client::addSettingsItemForMeshToSend(String key, int value){
    updatedSettingsObject[key] = value;
    hasUpdatedSinceLastSend = true;
}
void Mesh_Client::addSettingsItemForMeshToSend(String key, double value){
    updatedSettingsObject[key] = value;
    hasUpdatedSinceLastSend = true;
}
void Mesh_Client::addSettingsItemForMeshToSend(String key, unsigned int value){
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
void Mesh_Client::addBeacon(String key, uint8_t major, uint8_t minor, double distance){

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
 * @brief Add groupd of beacons to the updated settings
 * 
 * @param majors major values of beacon
 * @param minors minor values of beacon
 * @param distances between beacons and device
 */
void Mesh_Client::addBeacons(uint8_t majors[], uint8_t minors[], double distances[]){

    // std::lock_guard<std::mutex> lck(json_settings_mutex);

    for(int i = 0; i < 4; i++){

        JsonObject obj;

        if(updatedSettingsObject.containsKey(beaconKeys[i])){
            obj = updatedSettingsObject[beaconKeys[i]];
        }else{
            obj = updatedSettingsObject.createNestedObject(beaconKeys[i]);
        }

        obj["major"] = majors[i];
        obj["minor"] = minors[i];
        obj["distance"] = distances[i];
    }
}

/**
 * @brief Broadcast to the network the need to find bridge
 * 
 */
void Mesh_Client::askForBridge(void){
    // TODO replace this with proper enum
    mesh.sendBroadcast("{ \"FIND_BRIDGE\" : true }", true);
}

/**
 * @brief Function to update mesh in main loop
 * 
 */
void Mesh_Client::loopMesh(){
    mesh.update();
}