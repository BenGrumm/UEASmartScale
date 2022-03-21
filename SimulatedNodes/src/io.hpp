#ifndef IO_HEADER
#define IO_HEADER

#include <Arduino.h>
#include "painlessMesh.h"
#include "node_simulator.hpp"

#define PIN_BUTTON_1    34 // GPIO34
#define PIN_BUTTON_2    35 // GPIO34
#define PIN_LED_RED     23 // GPIO23
#define PIN_LED_BLUE    22 // GPIO22

void setupIO(Scheduler userScheduler);

void button1Press(void);
void button2Press(void);

void startBlueFlash(void);
void stopBlueFlash(void);

void startRedFlash(void);
void stopRedFlash(void);

void flashBlueLed(void);
void flashRedLed(void);

void setBlueLED(void);

#endif