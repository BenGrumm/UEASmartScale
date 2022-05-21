#ifndef NODE_SIMULATOR_HPP
#define NODE_SIMULATOR_HPP

#include "painlessMesh.h"
#include "mesh_client.hpp"
#include <ArduinoJson.h>

struct Settings{
    float calibrationVal;
    long zeroFactor;
    unsigned int numItemsPerWeight;
    double referenceWeight;

    uint32_t bridgeID;  
};

void setupSimulator(Scheduler &scheduler);
void updateRandomNumStored(void);
void updateRandomSettings(void);
void gotSettingsFromServer(JsonObject obj);

#endif