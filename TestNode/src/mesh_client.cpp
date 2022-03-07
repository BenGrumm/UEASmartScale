#include "mesh_client.hpp"

painlessMesh  mesh;
IPAddress myIP(0,0,0,0);

uint32_t bridgeID = 0;

unsigned int lastNumItemsSent = 941; // Random initializer as value likely to be 0 at setup

void setupMesh(Scheduler &userScheduler){
    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

    // Channel set to 6. Make sure to use the same channel for your mesh and for you other
    // network (STATION_SSID)
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6);
    // mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);

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
    }else if(root.containsKey(RECIEVED_UPDATED_SETTINGS)){
        // TODO Read update settings locally
        // Return ack
    }else if(root.containsKey(SERVER_RECIEVED_SETTINGS)){
        clearSettings();
    }
}

void clearSettings(void){
    Serial.println("Clearing settings");
    // if(!hasUpdatedSinceLastSend){
    //     Serial.println("Have Cleared");
    //     updatedSettingsObject.clear();
    // }
}

void sendUpdatedSettings(String msg){
    Serial.println("Updating");

    if(checkIfBridgeExists()){

        Serial.println("Is Settings To Update And Bridge Exists");
        
        if(bridgeID != mesh.getNodeId()){
            String finishedMSG = "{\"UPDATE_SETTINGS\":" + msg + "}";
            Serial.println(finishedMSG);        
            mesh.sendSingle(bridgeID, finishedMSG);
        }

    }else{
        Serial.println("Bridge not exists");
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

void askForBridge(void){
    // TODO replace this with proper enum
    mesh.sendBroadcast("{ \"FIND_BRIDGE\" : true }", true);
}

void loopMesh(){
    mesh.update();
}