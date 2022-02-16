#include "storage.hpp"

Preferences preferences;

void saveWeight(double weight){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putDouble(SETTINGS_NUM_WEIGHT, weight);

    preferences.end();
}

double getWeight(void){
    // Open namespace for read only
    preferences.begin(PREF_NAMESPACE, true);

    double val = preferences.getDouble(SETTINGS_NUM_WEIGHT, 0);

    preferences.end();

    return val;
}

void setCalibrationVal(float calVal){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putFloat(SETTINGS_CAL_VALUE, calVal);

    preferences.end();
}

float getCalibrationVal(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    double val = preferences.getFloat(SETTINGS_CAL_VALUE, 1);

    preferences.end();

    return val;
}

void setZeroFactor(long zeroFactor){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putLong(SETTINGS_ZERO_FACTOR, zeroFactor);

    preferences.end();
}

long getZeroFactor(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    long val = preferences.getLong(SETTINGS_ZERO_FACTOR, 0);

    preferences.end();

    return val;
}

void setNumItemsPerWeightVal(unsigned int numItems){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putUInt(SETTINGS_NUM_VALUES_PER, numItems);

    preferences.end();
}

unsigned int getNumItemsPerWeightVal(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    unsigned int val = preferences.getUInt(SETTINGS_NUM_VALUES_PER, 1);

    preferences.end();

    return val;
}

void setReferenceWeightOfItems(double itemsWeightGrams){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putDouble(SETTINGS_WEIGHT_REFERENCE, itemsWeightGrams);

    preferences.end();
}

double getReferenceWeightOfItemsGrams(void){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, true);

    double val = preferences.getDouble(SETTINGS_WEIGHT_REFERENCE, 10);

    preferences.end();

    return val;
}
