#include "mesh_client.hpp"

painlessMesh  mesh;
IPAddress myIP(0,0,0,0);

DynamicJsonDocument updatedSettings(2048);
DynamicJsonDocument upadtedItemCount(1024);

JsonObject updatedSettingsObject;
JsonObject updatedItemCountObjects;

Task periodicSettingsUpdates(TASK_SECOND * 30, TASK_FOREVER, &sendUpdatedSettings);

unsigned int lastNumItemsSent = 941; // Random initializer as value likely to be 0 at setup

void setupMesh(Scheduler &userScheduler){
    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

    // Channel set to 6. Make sure to use the same channel for your mesh and for you other
    // network (STATION_SSID)
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6);
    // mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6);
    mesh.onReceive(&receivedCallback);

    // To connect to wifi
    mesh.stationManual(STATION_SSID, STATION_PASSWORD);
    mesh.setHostname(HOSTNAME);
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    mesh.setRoot(true);
    // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
    mesh.setContainsRoot(true);

    updatedSettingsObject = updatedSettings.createNestedObject(UPDATE_SETTINGS);

    if(deviceSettings.initialisation){
        addSettingsItemForMeshToSend(NUM_ITEMS_PER_WEIGHT_KEY, deviceSettings.numItemsPerWeight);
        addSettingsItemForMeshToSend(WEIGHT_PER_X_KEY, deviceSettings.referenceWeight);
    }

    updatedItemCountObjects = upadtedItemCount.createNestedObject(UPDATE_NUM_ITEMS);

    userScheduler.addTask(periodicSettingsUpdates);
    periodicSettingsUpdates.enableDelayed(TASK_SECOND * 30);
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

    if(root.containsKey(FIND_BRIDGE) && mesh.isRoot()){
        String outputMsg;
        StaticJsonDocument<1024> jsonBuffer;
        JsonObject obj = jsonBuffer.createNestedObject();
        obj[BRIDGE_KNOWN] = mesh.getNodeId();
        serializeJson(obj, outputMsg);
        
        mesh.sendBroadcast(outputMsg, true);
    }else if(root.containsKey(UPDATE_SETTINGS) && mesh.isRoot()){
        // Bridge has recieved a nodes updated settings add to list
        JsonObject obj = root[UPDATE_SETTINGS];
        addUpdatedSettings(obj);
    }else if(root.containsKey(BRIDGE_KNOWN)){
        if(deviceSettings.bridgeID != root[BRIDGE_KNOWN]){
            setBridgeID(root[BRIDGE_KNOWN]);
        }
    }else if(root.containsKey(RECIEVED_UPDATED_SETTINGS)){
        // TODO Read update settings locally
        // Return ack
    }
}

void sendUpdatedSettings(void){
    if(updatedSettingsObject.size() > 0 && checkIfBridgeExists()){
        Serial.println("In Mesh Settings Update");
        updatedSettingsObject["id"] = mesh.getNodeId();
        
        if(deviceSettings.bridgeID != mesh.getNodeId()){
            String msg;
            serializeJson(updatedSettingsObject, msg);
            Serial.println(msg);
        
            Serial.println("Send to bridge");
            mesh.sendSingle(deviceSettings.bridgeID, msg);
        }else{
            Serial.println("Am Bridge");
            addUpdatedSettings(updatedSettingsObject);
        }
        updatedSettingsObject.clear();
    }
    serializeJsonPretty(updatedSettingsObject, Serial); Serial.println();
}

void sendNumStored(void){
    if(updatedItemCountObjects.size() > 0 && updatedItemCountObjects["numStored"] != lastNumItemsSent && checkIfBridgeExists()){
        String msg;
        updatedItemCountObjects["id"] = mesh.getNodeId();
        serializeJson(updatedItemCountObjects, msg);
        mesh.sendSingle(deviceSettings.bridgeID, msg);
        updatedItemCountObjects.clear();
    }
}

bool checkIfBridgeExists(void){
    if(deviceSettings.bridgeID != 0){
        SimpleList<uint32_t> nl = mesh.getNodeList(true);
        SimpleList<uint32_t>::iterator itr = nl.begin();

        while(itr != nl.end()) {
            if(*itr == deviceSettings.bridgeID){
                Serial.print("Found Bridge ID - ");
                Serial.println(*itr);
                return true;
            }
            itr++;
        }
    }

    askForBridge();

    return false;
}

void updateNumStored(unsigned int numItems){
    updatedItemCountObjects["numStored"] = numItems;
}

void addSettingsItemForMeshToSend(String key, String value){
    updatedSettingsObject[key] = value;
}

void addSettingsItemForMeshToSend(String key, int value){
    updatedSettingsObject[key] = value;
}

void addSettingsItemForMeshToSend(String key, double value){
    updatedSettingsObject[key] = value;
}

void addSettingsItemForMeshToSend(String key, unsigned int value){
    updatedSettingsObject[key] = value;
}


void askForBridge(void){
    // TODO replace this with proper enum
    mesh.sendBroadcast("{ \"FIND_BRIDGE\" : true }", true);
}

void loopMesh(){
    mesh.update();
}