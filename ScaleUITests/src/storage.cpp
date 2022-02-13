#include <Arduino.h>
#include <Preferences.h>

#define PREF_NAMESPACE "settings"

#define SETTINGS_NUM_WEIGHT "NUM_WEIGHT"
#define SETTINGS_CAL_VALUE "CAL_VALUE"
#define SETTINGS_ZERO_FACTOR "ZERO_FACTOR"

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