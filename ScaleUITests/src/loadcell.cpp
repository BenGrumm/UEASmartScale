#include "loadcell.hpp"

HX711 scale;
unsigned int userKnowWeight = 1000; // Known weight in grams

void setupLC(unsigned int dataPin, unsigned int clkPin){
    scale.begin(dataPin, clkPin);

    // Retrieve stored vals (if any)
    long zeroFac = getZeroFactor();
    float calVal = getCalibrationVal();

    Serial.print("Zero Factor = ");
    Serial.println(zeroFac);
    Serial.print("Calibration Val = ");
    Serial.println(calVal);

    // Configure with stored vals
    scale.set_scale(calVal);
    scale.set_offset(zeroFac); //Used for taring / zeroing
}

void zeroTare(void){
    Serial.println("Zero");
    long avg = scale.read_average();

    setZeroFactor(avg);

    scale.set_offset(avg);
}

double getWeightGrams(void){
    return scale.get_units();
}

void readScale(void){
    saveWeight(scale.get_units());
}

void calibrateScale(void){
    Serial.println("Calibrate");
    scale.set_scale(1);
    double calibrationFactor = scale.get_units(10) / userKnowWeight;
    scale.set_scale(calibrationFactor);

    setCalibrationVal(calibrationFactor);
}

void setKnownWeight(unsigned int knownVal){
    userKnowWeight = knownVal;
}
