#include "mesh_client.hpp"

painlessMesh  mesh;
IPAddress myIP(0,0,0,0);

DynamicJsonDocument updatedSettings(2048);
bool hasUpdatedSinceLastSend = false;
bool bridgeExists = false;

JsonObject updatedSettingsObject;
JsonObject updatedItemCountObjects;

Task periodicSettingsUpdates(TASK_SECOND * 30, TASK_FOREVER, &sendUpdatedSettings);

unsigned int lastNumItemsSent = 941; // Random initializer as value likely to be 0 at setup

void setupMesh(Scheduler &userScheduler){
    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

    // Channel set to 6. Make sure to use the same channel for your mesh and for you other
    // network (STATION_SSID)
    mesh.init("SCALE_NETWORK_" + deviceSettings.meshName, "MESH_PASS_" + deviceSettings.meshPassword, &userScheduler, MESH_PORT, WIFI_AP_STA, 6);
    // mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6);
    mesh.onReceive(&receivedCallback);

    #ifdef ROOT

    Serial.println("Setup Mesh WIFI");

    // To connect to wifi
    // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
    Serial.println("SSID " + deviceSettings.WIFISSID + ", Password = " + deviceSettings.WIFIPassword);
    mesh.stationManual(deviceSettings.WIFISSID, deviceSettings.WIFIPassword);
    mesh.setHostname(HOSTNAME);
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    mesh.setRoot(true);
    // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
    mesh.setContainsRoot(true);

    #endif

    updatedSettingsObject = updatedSettings.createNestedObject(UPDATE_SETTINGS);

    if(deviceSettings.initialisation){
        addSettingsItemForMeshToSend(NUM_ITEMS_PER_WEIGHT_KEY, deviceSettings.numItemsPerWeight);
        addSettingsItemForMeshToSend(WEIGHT_PER_X_KEY, deviceSettings.referenceWeight);
    }

    userScheduler.addTask(periodicSettingsUpdates);
    periodicSettingsUpdates.enableDelayed(TASK_SECOND * 15);

}

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
        // Return ack
    }
    // The settings sent to root from this node successfully updated on the server
    else if(root.containsKey(SERVER_RECIEVED_SETTINGS)){
        clearSettings();
    }
}

void clearSettings(void){
    Serial.println("Clearing settings");
    if(!hasUpdatedSinceLastSend){
        Serial.println("Have Cleared");
        updatedSettingsObject.clear();
    }
}

void sendUpdatedSettings(void){
    if(updatedSettingsObject.size() > 0 && checkIfBridgeExists()){
        Serial.println("Is Settings To Update And Bridge Exists");
        serializeJsonPretty(updatedSettingsObject, Serial); Serial.println();
        updatedSettingsObject["id"] = mesh.getNodeId();
        
        if(deviceSettings.bridgeID != mesh.getNodeId()){
            String msg;
            serializeJson(updatedSettingsObject, msg);
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

void rootSendUpdateAck(uint32_t id){
    if(id == mesh.getNodeId()){
        clearSettings();
    }else{
        mesh.sendSingle(id, "{\"SERVER_RECIEVED_SETTINGS\":true}");
    }
}

bool checkIfBridgeExists(void){
    if(deviceSettings.bridgeID != 0){
        SimpleList<uint32_t> nl = mesh.getNodeList(true);
        SimpleList<uint32_t>::iterator itr = nl.begin();

        while(itr != nl.end()) {
            if(*itr == deviceSettings.bridgeID){
                Serial.print("Checked And Found Bridge ID In List - ");
                Serial.println(*itr);
                bridgeExists = true;
                return true;
            }
            itr++;
        }
    }

    askForBridge();

    bridgeExists = false;

    return false;
}

bool getIfBridgeExists(void){
    return bridgeExists;
}

int getNumConnectedNodes(void){
    return mesh.getNodeList(true).size();
}

void updateNumStored(unsigned int numItems){
    updatedSettingsObject[NUM_STORED_KEY] = numItems;
    hasUpdatedSinceLastSend = true;
}

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


void askForBridge(void){
    // TODO replace this with proper enum
    mesh.sendBroadcast("{ \"FIND_BRIDGE\" : true }", true);
}

void loopMesh(){
    mesh.update();
}