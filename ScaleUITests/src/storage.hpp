#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <Preferences.h>

#define PREF_NAMESPACE "settings"

#define SETTINGS_NUM_WEIGHT "NUM_WEIGHT"
#define SETTINGS_CAL_VALUE "CAL_VALUE"
#define SETTINGS_ZERO_FACTOR "ZERO_FACTOR"
#define SETTINGS_NUM_VALUES_PER "NUM_VALUES_PER"

// Load cell functions

/**
 * @brief Save the weight scale to memory
 * 
 * @param weight in grams
 */
void saveWeight(double weight);
/**
 * @brief Get the Weight object
 * 
 * @return double of weight (if none stored default = 0)
 */
double getWeight(void);

/**
 * @brief Save the calibration value to storage
 * 
 * @param calVal val to save
 */
void setCalibrationVal(float calVal);
/**
 * @brief Get the calibration value
 * 
 * @return float of value stored (if none default = 1)
 */
float getCalibrationVal(void);

/**
 * @brief Set the zero factor value
 * 
 * @param zeroFactor zero factor calibration
 */
void setZeroFactor(long zeroFactor);
/**
 * @brief Get the zero factor object
 * 
 * @return long zero factor stored (if none default = 0)
 */
long getZeroFactor(void);

/**
 * @brief Set the number of items per weight value
 * 
 * @param numItems num items to be stored
 */
void setNumItemsPerWeightVal(unsigned int numItems);
/**
 * @brief Get the number of items per weight value
 * 
 * @return unsigned int with number item per weight (if none stored default = 1)
 */
unsigned int getNumItemsPerWeightVal(void);

#endif