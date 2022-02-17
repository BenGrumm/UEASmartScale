#include <Arduino.h>
#include "ui.hpp"
#include "loadcell.hpp"
#include "settings_server.hpp"
#include "mesh_client.hpp"

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

  setupMesh(userScheduler);
  setupLC(DOUT, CLK);
  // LCD uses defualt i2c pins
  setupUI(userScheduler, BUTTON_1, BUTTON_2, BUTTON_3);
  setupServer(getMeshAPIP());

}

void loop(){
  // This will update the mesh which also updates the scheduler that is passed
  loopMesh();
}
