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

  setupMesh(userScheduler);
  setupLC(userScheduler, DOUT, CLK);
  // LCD uses defualt i2c pins
  setupUI(userScheduler, BUTTON_1, BUTTON_2, BUTTON_3);
  #ifdef ROOT
  Serial.println("ROOT Def");
  setupServer(getMeshAPIP());
  setupHTTP(userScheduler);
  #endif
  setupBTScan(userScheduler);

  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());
}

void loop(){
  // This will update the mesh which also updates the scheduler that is passed
  loopMesh();
}
