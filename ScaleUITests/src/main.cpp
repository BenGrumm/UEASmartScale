#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include "ui.hpp"

Scheduler userScheduler;

void setup()
{
  Serial.begin(9600);

  setupUI(userScheduler);
}

void loop(){
  userScheduler.execute();
}
