#include "mesh_client.hpp"

painlessMesh  mesh;
IPAddress myIP(0,0,0,0);

DynamicJsonDocument updatedSettings(512);
JsonObject updatedSettingsObject;

bool hasUpdatedSinceLastSend = false;
uint32_t bridgeID = 0;

Task periodicSettingsUpdates(TASK_SECOND * 30, TASK_FOREVER, &sendUpdatedSettings);
Task checkIfConnected(TASK_SECOND * 5, TASK_FOREVER, &whenConnected);

void setupMesh(Scheduler &userScheduler){
    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

    // Channel set to 6. Make sure to use the same channel for your mesh and for you other
    // network (STATION_SSID)
    mesh.init("MESH_NETWORK_SCALE", "MESH_PASS_12345", &userScheduler, MESH_PORT, WIFI_AP_STA, 6);
    // mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);

    userScheduler.addTask(checkIfConnected);
    checkIfConnected.enable();

    userScheduler.addTask(periodicSettingsUpdates);
    periodicSettingsUpdates.enableDelayed(TASK_SECOND * 15);

    updatedSettingsObject = updatedSettings.createNestedObject(UPDATE_SETTINGS);
}

void whenConnected(void){
    if(mesh.getNodeList().size() > 0){
        // Stop
        checkIfConnected.disable();
        // Set the light
        setBlueLED();
    }
}

IPAddress getMeshAPIP(void){
    return IPAddress(mesh.getAPIP());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
    askForBridge();
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

    if(root.containsKey(BRIDGE_KNOWN)){
        // Recieved the id of bridge on the current network
        if(bridgeID != root[BRIDGE_KNOWN]){
            bridgeID = root[BRIDGE_KNOWN];
        }
    }
    // Received a message giving updated settings
    else if(root.containsKey(RECIEVED_UPDATED_SETTINGS)){
        // TODO Read update settings locally
        gotSettingsFromServer(root[RECIEVED_UPDATED_SETTINGS]);
        // Return ack
        ackUpdatedSettings();
    }
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
    Serial.println("Updating");

    if(updatedSettingsObject.size() > 0 && checkIfBridgeExists()){
        Serial.println("Is Settings To Update And Bridge Exists");
        serializeJsonPretty(updatedSettingsObject, Serial); Serial.println();
        updatedSettingsObject["id"] = mesh.getNodeId();
        
        if(bridgeID != mesh.getNodeId()){
            String msg;
            serializeJson(updatedSettings, msg);
            Serial.println(msg);
        
            Serial.println("Not Bridge So Send to bridge");
            mesh.sendSingle(bridgeID, msg);
        }

        hasUpdatedSinceLastSend = false;
    }

}

bool checkIfBridgeExists(void){
    if(bridgeID != 0){
        SimpleList<uint32_t> nl = mesh.getNodeList(true);
        SimpleList<uint32_t>::iterator itr = nl.begin();

        while(itr != nl.end()) {
            if(*itr == bridgeID){
                Serial.print("Checked And Found Bridge ID In List - ");
                Serial.println(*itr);
                return true;
            }
            itr++;
        }
    }

    askForBridge();

    return false;
}

void ackUpdatedSettings(void){
    if(!mesh.isRoot()){
        mesh.sendSingle(bridgeID, "{\"ACK_RECEIVED\":true");
    }
}

void askForBridge(void){
    // TODO replace this with proper enum
    mesh.sendBroadcast("{ \"FIND_BRIDGE\" : true }", true);
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

void loopMesh(){
    mesh.update();
}