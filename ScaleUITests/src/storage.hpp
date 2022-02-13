#ifndef STORAGE_HPP
#define STORAGE_HPP

// Load cell functions
void saveWeight(double weight);
double getWeight(void);

void setCalibrationVal(float calVal);
float getCalibrationVal(void);

void setZeroFactor(long zeroFactor);
long getZeroFactor(void);

#endif