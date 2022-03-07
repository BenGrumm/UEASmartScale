#include "node_simulator.hpp"

DynamicJsonDocument document(512 * NUM_SIMULATED_NODES);

JsonObject objects[NUM_SIMULATED_NODES];

Task createObjects(TASK_SECOND, NUM_SIMULATED_NODES, &generateInitialData);

void setupSimulator(Scheduler &scheduler){
    randomSeed(analogRead(3));

    scheduler.addTask(createObjects);

    createObjects.enableDelayed(TASK_SECOND * 20);
}

int i = 0;

void generateInitialData(){
    Serial.printf("Generating Data");
    objects[i] = document.createNestedObject();
    objects[i]["id"] = i;
    objects[i][NUM_STORED_KEY] = 100;
    objects[i][NUM_ITEMS_PER_WEIGHT_KEY] = 10;
    objects[i][WEIGHT_PER_X_KEY] = 5;
    objects[i][MIN_NUM_ITEMS_KEY] = 500;
    objects[i][OK_NUM_ITEMS_KEY] = 5000;

    String msg;

    serializeJson(objects[i++], msg);

    sendUpdatedSettings(msg);

    createObjects.setInterval(random(100, 300));
}