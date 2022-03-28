#include "storage.hpp"

Preferences preferences;
Settings deviceSettings;

void setupStorage(void){
    preferences.begin(PREF_NAMESPACE, true);

    // Assign all vars from storage to a struct
    deviceSettings.calibrationVal = preferences.getFloat(SETTINGS_CAL_VALUE, 1);
    deviceSettings.numItemsPerWeight = preferences.getUInt(SETTINGS_NUM_VALUES_PER, 1);
    deviceSettings.referenceWeight = preferences.getDouble(SETTINGS_WEIGHT_REFERENCE, 10);
    deviceSettings.zeroFactor = preferences.getLong(SETTINGS_ZERO_FACTOR, 0);

    deviceSettings.username = preferences.getString(SETTINGS_SERVER_USERNAME, "Ben12");
    deviceSettings.password = preferences.getString(SETTINGS_SERVER_PASSWORD, "test");
    deviceSettings.jwt = preferences.getString(SETTINGS_JWT, "");

    deviceSettings.WIFISSID = preferences.getString(SETTINGS_WIFI_SSID, "");
    deviceSettings.WIFIPassword = preferences.getString(SETTINGS_WIFI_PASSWORD, "");

    deviceSettings.meshName = preferences.getString(SETTINGS_MESH_NAME, "SCALE");
    deviceSettings.meshPassword = preferences.getString(SETTINGS_MESH_PASSWORD, "12345");

    deviceSettings.initialisation = preferences.getBool(INITIALISATION, true);

    preferences.end();

    // If debug needed
    // Serial.println("Settings");
    // Serial.print("Cal Val ");Serial.println(deviceSettings.calibrationVal);
    // Serial.print("Num Items Per ");Serial.println(deviceSettings.numItemsPerWeight);
    // Serial.print("Ref Weight ");Serial.println(deviceSettings.referenceWeight);
    // Serial.print("Zero ");Serial.println(deviceSettings.zeroFactor);
}

void setBridgeID(uint32_t id){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.bridgeID = id;
    preferences.putUInt(SETTINGS_BRIDGE_ID, id);

    preferences.end();
}

uint32_t getBridgeIDMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    uint32_t val = preferences.getUInt(SETTINGS_BRIDGE_ID, 0);

    preferences.end();

    return val;
}

void setSSID(String SSID){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.WIFISSID = SSID;
    preferences.putString(SETTINGS_WIFI_SSID, SSID);

    preferences.end();
}

String getSSIDMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_WIFI_SSID, "");

    preferences.end();

    return val;
}

void setWIFIPassword(String password){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.WIFIPassword = password;
    preferences.putString(SETTINGS_WIFI_PASSWORD, password);

    preferences.end();
}

String getWIFIPasswordMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_WIFI_SSID, "");

    preferences.end();

    return val;
}

void setUsername(String username){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.username = username;
    preferences.putString(SETTINGS_SERVER_USERNAME, username);

    preferences.end();
}

String getUsernameMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_SERVER_USERNAME, "");

    preferences.end();

    return val;
}

void setPassword(String password){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.password = password;
    preferences.putString(SETTINGS_SERVER_PASSWORD, password);

    preferences.end();
}

String getPasswordMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_SERVER_PASSWORD, "");

    preferences.end();

    return val;
}

void setJWT(String jwt){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.jwt = jwt;
    preferences.putString(SETTINGS_JWT, jwt);

    preferences.end();
}

String getJWTMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_JWT, "");

    preferences.end();

    return val;
}

void setCalibrationVal(float calVal){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.calibrationVal = calVal;
    preferences.putFloat(SETTINGS_CAL_VALUE, calVal);

    preferences.end();
}

float getCalibrationValMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    double val = preferences.getFloat(SETTINGS_CAL_VALUE, 1);

    preferences.end();

    return val;
}

void setZeroFactor(long zeroFactor){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.zeroFactor = zeroFactor;
    preferences.putLong(SETTINGS_ZERO_FACTOR, zeroFactor);

    preferences.end();
}

long getZeroFactorMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    long val = preferences.getLong(SETTINGS_ZERO_FACTOR, 0);

    preferences.end();

    return val;
}

void setNumItemsPerWeightVal(unsigned int numItems){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.numItemsPerWeight = numItems;
    preferences.putUInt(SETTINGS_NUM_VALUES_PER, numItems);
    addSettingsItemForMeshToSend(NUM_ITEMS_PER_WEIGHT_KEY, numItems);

    preferences.end();
}

unsigned int getNumItemsPerWeightValMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    unsigned int val = preferences.getUInt(SETTINGS_NUM_VALUES_PER, 1);

    preferences.end();

    return val;
}

void setReferenceWeightOfItems(double itemsWeightGrams){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    deviceSettings.referenceWeight = itemsWeightGrams;
    preferences.putDouble(SETTINGS_WEIGHT_REFERENCE, itemsWeightGrams);
    addSettingsItemForMeshToSend(WEIGHT_PER_X_KEY, itemsWeightGrams);

    preferences.end();
}

double getReferenceWeightOfItemsGramsMem(void){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, true);

    double val = preferences.getDouble(SETTINGS_WEIGHT_REFERENCE, 10);

    preferences.end();

    return val;
}

void saveWeight(double weight){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putDouble(SETTINGS_NUM_WEIGHT, weight);

    preferences.end();
}

double getWeightMem(void){
    // Open namespace for read only
    preferences.begin(PREF_NAMESPACE, true);

    double val = preferences.getDouble(SETTINGS_NUM_WEIGHT, 0);

    preferences.end();

    return val;
}

void saveMeshName(String name){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putString(SETTINGS_MESH_NAME, name);

    preferences.end();
}

String getMeshName(void){
    // Open namespace for read only
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_MESH_NAME, "SCALE");

    preferences.end();

    return val;
}

void saveMeshPassword(String password){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putString(SETTINGS_MESH_PASSWORD, password);

    preferences.end();
}

String getMeshPassword(void){
    // Open namespace for read only
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_MESH_PASSWORD, "12345");

    preferences.end();

    return val;
}
