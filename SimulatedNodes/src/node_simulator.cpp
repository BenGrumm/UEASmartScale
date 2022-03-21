#include "node_simulator.hpp"

void setupSimulator(Scheduler &scheduler){    
    updateNumStored(1);
    addSettingsItemForMeshToSend(NUM_ITEMS_PER_WEIGHT_KEY, 10);
    addSettingsItemForMeshToSend(WEIGHT_PER_X_KEY, 5);
    addSettingsItemForMeshToSend(MIN_NUM_ITEMS_KEY, 500);
    addSettingsItemForMeshToSend(OK_NUM_ITEMS_KEY, 5000);
}

void updateRandomNumStored(void){
    Serial.println("Updating Num Stored");
}

void updateRandomSettings(void){
    Serial.println("Updating Settings");
}