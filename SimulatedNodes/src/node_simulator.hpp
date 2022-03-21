#ifndef NODE_SIMULATOR_HPP
#define NODE_SIMULATOR_HPP

#include "painlessMesh.h"
#include "mesh_client.hpp"
#include <ArduinoJson.h>

void setupSimulator(Scheduler &scheduler);
void updateRandomNumStored(void);
void updateRandomSettings(void);

#endif