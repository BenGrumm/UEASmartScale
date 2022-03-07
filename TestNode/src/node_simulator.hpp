#ifndef NODE_SIMULATOR_HPP
#define NODE_SIMULATOR_HPP

#include "painlessMesh.h"
#include "mesh_client.hpp"
#include <ArduinoJson.h>

#define NUM_SIMULATED_NODES 50

void setupSimulator(Scheduler &scheduler);
void generateInitialData(void);

#endif