#ifndef LOADCELL_H
#define LOADCELL_H

#include <Arduino.h>
#include <math.h>
#include "HX711.h"
#include "storage.hpp"
#include "painlessMesh.h"

/**
 * @brief Function to calculate the number of objects on scale from weight in grams and the reference weight and num items stored in memory
 * 
 * @return unsigned int number of items on the scale
 */
unsigned int getNumItems(void);

/**
 * @brief Function used for setup of the load cell
 * 
 * @param userScheduler 
 * @param dataPin esp pin number for data
 * @param clkPin esp pin number for the clock
 */
void setupLC(Scheduler &userScheduler, unsigned int dataPin, unsigned int clkPin);

/**
 * @brief When called will zero the scale with the current weight on it
 * 
 */
void zeroTare(void);

/**
 * @brief Get the weight on the scale in grams
 * 
 * @return double 
 */
double getWeightGrams(void);

/**
 * @brief Read the scale value and store in memory
 * 
 */
void readScale(void);

/**
 * @brief Will calibrate the scale based on weight current on scale and
 *        value stored as the known val
 * 
 */
void calibrateScale(void);

/**
 * @brief Set the Known Weight object
 * 
 * @param knownVal value to store
 */
void setKnownWeight(unsigned int knownVal);

#endif