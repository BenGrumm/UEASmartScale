#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TaskSchedulerDeclarations.h>
#include <math.h>

// 
void setupUI(Scheduler &userScheduler);

#endif