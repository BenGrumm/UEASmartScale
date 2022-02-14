#ifndef STORAGE_HPP
#define STORAGE_HPP

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

#endif