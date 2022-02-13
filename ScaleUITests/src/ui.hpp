#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TaskSchedulerDeclarations.h>
#include <math.h>

/**
 * @brief Function used to setup the UI pins and tasks
 * 
 * @param userScheduler global scheduler called on every loop
 * @param button1Pin pin number for the button 1 input
 * @param button2Pin pin number for the button 2 input
 * @param button3Pin pin number for the button 3 input
 */
void setupUI(Scheduler &userScheduler, int button1Pin, int button2Pin, int button3Pin);

#endif