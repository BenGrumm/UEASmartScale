#ifndef LOADCELL_H
#define LOADCELL_H

#include <Arduino.h>
#include "HX711.h"
#include "storage.hpp"
#include <TaskSchedulerDeclarations.h>

void setupLC(unsigned int dataPin, unsigned int clkPin);
void zeroTare(void);
double getWeightGrams(void);
void readScale(void);
void calibrateScale(void);
void setKnownWeight(unsigned int knownVal);

#endif