#include "loadcell.hpp"

HX711 scale;
unsigned int userKnowWeight = 1000; // Known weight in grams

Task checkAndUpdateScaleNum(TASK_SECOND * 30, TASK_FOREVER, &checkNumItemsAndUpdate);

void setupLC(Scheduler &userScheduler, unsigned int dataPin, unsigned int clkPin){
    scale.begin(dataPin, clkPin);

    // Retrieve stored vals (if any)
    long zeroFac = deviceSettings.zeroFactor;
    float calVal = deviceSettings.calibrationVal;

    Serial.print("Zero Factor = ");
    Serial.println(zeroFac);
    Serial.print("Calibration Val = ");
    Serial.println(calVal);

    // Configure with stored vals
    scale.set_scale(calVal);
    scale.set_offset(zeroFac); //Used for taring / zeroing

    userScheduler.addTask(checkAndUpdateScaleNum);
    checkAndUpdateScaleNum.enableDelayed(TASK_SECOND * 10);
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

/**
 * @brief Function to calculate the number of objects on scale from weight in grams and the reference weight and num items stored in memory
 * 
 * @return unsigned int number of items on the scale
 */
double getNumItems(void){
    double ref = deviceSettings.referenceWeight;
    unsigned int numItems = deviceSettings.numItemsPerWeight;
    return round(getWeightGrams() / (ref / numItems));
}

unsigned int lastKnownWeight = 2868;

void checkNumItemsAndUpdate(void){
    double currentNum = getNumItems();

    if(currentNum < 0){
        currentNum = 0;
    }

    if(lastKnownWeight != currentNum){
        updateNumStored(currentNum);
        lastKnownWeight = currentNum;
    }
}
