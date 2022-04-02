#include "node_simulator.hpp"

Settings deviceSettings;
int numStored = 1;

void setupSimulator(Scheduler &scheduler){

    deviceSettings.calibrationVal = 1;
    deviceSettings.numItemsPerWeight = 1;
    deviceSettings.referenceWeight = 10;
    deviceSettings.zeroFactor = 0;

    updateNumStored(numStored);
    addSettingsItemForMeshToSend(NUM_ITEMS_PER_WEIGHT_KEY, deviceSettings.numItemsPerWeight);
    addSettingsItemForMeshToSend(WEIGHT_PER_X_KEY, deviceSettings.referenceWeight);
    addSettingsItemForMeshToSend(MIN_NUM_ITEMS_KEY, 500);
    addSettingsItemForMeshToSend(OK_NUM_ITEMS_KEY, 5000);
}

void updateRandomNumStored(void){
    Serial.println("Updating Num Stored");
    updateNumStored(++numStored);
}

void updateRandomSettings(void){
    Serial.println("Updating Settings");

    addSettingsItemForMeshToSend(NUM_ITEMS_PER_WEIGHT_KEY, ++deviceSettings.numItemsPerWeight);
    addSettingsItemForMeshToSend(WEIGHT_PER_X_KEY, ++deviceSettings.referenceWeight);
}

void gotSettingsFromServer(JsonObject obj){
    
}