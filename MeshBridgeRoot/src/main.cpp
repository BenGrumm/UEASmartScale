//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define   STATION_SSID     "TP-LINK_947990"
#define   STATION_PASSWORD "44020712"

#define HOSTNAME "HTTP_BRIDGE"

void makeRequest(void);

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

DynamicJsonDocument doc(1024);

unsigned long lastTime = 0;
double btcPriceGBP = 0;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hello from bridge node ";
  msg += mesh.getNodeId();
  msg += (", price = $" + String(btcPriceGBP));
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval(TASK_SECOND * 30);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
  
  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();

  if ((millis() - lastTime) > 30000) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      makeRequest();
    }else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void makeRequest(){
  HTTPClient http;
  
  http.begin("https://api.coindesk.com/v1/bpi/currentprice.json");

  int httpCode = http.GET();

  if(httpCode > 0) {
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      deserializeJson(doc, payload);
      btcPriceGBP = doc["bpi"]["GBP"]["rate_float"];
      Serial.printf("BTC Price = $%f\n", btcPriceGBP);
    }
  }
  http.end();
}