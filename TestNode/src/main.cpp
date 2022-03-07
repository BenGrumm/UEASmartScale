#include <Arduino.h>
#include "mesh_client.hpp"
#include "node_simulator.hpp"

Scheduler userScheduler;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  setupMesh(userScheduler);

  setupSimulator(userScheduler);

}

void loop() {
  // put your main code here, to run repeatedly:
  loopMesh();
}