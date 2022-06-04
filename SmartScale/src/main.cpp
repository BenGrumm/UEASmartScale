#include <Arduino.h>
#include "ui.hpp"
#include "loadcell.hpp"
#include "settings_server.hpp"
#include "mesh_client.hpp"
#include "bt_scanner.hpp"

#ifdef ROOT
#include "http_requests.hpp"
#endif

// Ben Female ESP
// #define BUTTON_3    2 // GPIO34
// #define BUTTON_2    0 // GPIO23
// #define BUTTON_1    4 // GPIO22

// Ben Male ESP
#define BUTTON_3    4 // GPIO34
#define BUTTON_2    5 // GPIO23
#define BUTTON_1    15 // GPIO22

// For the HX711
#define DOUT  32
#define CLK  33

Scheduler userScheduler;

void setup(){
  Serial.begin(9600);

  // Serial.println("Getting Settings");
  // DeviceSettings* set = DeviceSettings::getInstance();

  Mesh_Client::setupMesh(userScheduler);
  LoadCell::setupLoadCell(userScheduler, DOUT, CLK);
  // LCD uses defualt i2c pins
  UI::setupUI(userScheduler, BUTTON_1, BUTTON_2, BUTTON_3);
  #ifdef ROOT
  Serial.println("ROOT Def");
  Settings_Server::setupServer(Mesh_Client::getMeshAPIP());
  HTTP_Requests::setupHTTP(userScheduler);
  #endif
  BT_Scanner::setupBTScan(userScheduler);

  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());
}

void loop(){
  // This will update the mesh which also updates the scheduler that is passed
  Mesh_Client::loopMesh();
}
