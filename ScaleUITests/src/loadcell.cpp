#include "loadcell.hpp"

HX711 scale;
unsigned int userKnowWeight = 1000;

void setupLC(unsigned int dataPin, unsigned int clkPin){
    scale.begin(dataPin, clkPin);

    long zeroFac = getZeroFactor();
    float calVal = getCalibrationVal();

    Serial.print("Zero Factor = ");
    Serial.println(zeroFac);
    Serial.print("Calibration Val = ");
    Serial.println(calVal);

    scale.set_scale(calVal);
    scale.set_offset(zeroFac); //Used for taring / zeroing
}

void zeroTare(void){
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
    scale.set_scale(1);
    double calibrationFactor = scale.get_units(10) / userKnowWeight;
    scale.set_scale(calibrationFactor);

    setCalibrationVal(calibrationFactor);
}

void setKnownWeight(unsigned int knownVal){
    userKnowWeight = knownVal;
}
