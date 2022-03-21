#include <Arduino.h>
#include "mesh_client.hpp"
#include "node_simulator.hpp"
#include "io.hpp"

#define SINGLE

Scheduler userScheduler;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  setupIO(userScheduler);

  setupMesh(userScheduler);

  setupSimulator(userScheduler);
}

void loop() {
  // put your main code here, to run repeatedly:
  loopMesh();
}