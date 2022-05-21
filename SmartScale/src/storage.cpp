#include "storage.hpp"

DeviceSettings::DeviceSettings(){
    Serial.println("Create Instance");

    preferences.begin(PREF_NAMESPACE, true);
    // preferences.clear();

    calibrationVal = preferences.getFloat(SETTINGS_CAL_VALUE, 1);
    numItemsPerWeight = preferences.getUInt(SETTINGS_NUM_VALUES_PER, 1);
    referenceWeight = preferences.getDouble(SETTINGS_WEIGHT_REFERENCE, 10);
    zeroFactor = preferences.getLong(SETTINGS_ZERO_FACTOR, 0);

    username = preferences.getString(SETTINGS_SERVER_USERNAME, "Ben12");
    password = preferences.getString(SETTINGS_SERVER_PASSWORD, "test");
    jwt = preferences.getString(SETTINGS_JWT, "");

    WIFISSID = preferences.getString(SETTINGS_WIFI_SSID, "");
    WIFIPassword = preferences.getString(SETTINGS_WIFI_PASSWORD, "");

    meshName = preferences.getString(SETTINGS_MESH_NAME, "SCALE");
    meshPassword = preferences.getString(SETTINGS_MESH_PASSWORD, "12345");

    initialisation = preferences.getBool(INITIALISATION, true);

    preferences.end();
}

void DeviceSettings::updateLocalSettings(JsonObject newSettings){
    if(newSettings.containsKey(WEIGHT_PER_X_KEY) && newSettings[WEIGHT_PER_X_KEY] != referenceWeight){
        setReferenceWeightOfItems(newSettings[WEIGHT_PER_X_KEY], false);
    }

    if(newSettings.containsKey(NUM_ITEMS_PER_WEIGHT_KEY) && newSettings[NUM_ITEMS_PER_WEIGHT_KEY] != numItemsPerWeight){
        setNumItemsPerWeightVal(newSettings[NUM_ITEMS_PER_WEIGHT_KEY], false);
    }

    // if(newSettings.containsKey(MIN_NUM_ITEMS_KEY) && newSettings[MIN_NUM_ITEMS_KEY] != deviceSettings.minNumItems){
    //     Serial.println("Update min num items");
    //     // set(newSettings[NUM_ITEMS_PER_WEIGHT_KEY]);
    // }

    // if(newSettings.containsKey(OK_NUM_ITEMS_KEY) && newSettings[OK_NUM_ITEMS_KEY] != deviceSettings.okNumItems){
    //     set(newSettings[OK_NUM_ITEMS_KEY]);
    // }
}

void DeviceSettings::setBridgeID(uint32_t id){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    bridgeID = id;
    preferences.putUInt(SETTINGS_BRIDGE_ID, id);

    preferences.end();
}

uint32_t DeviceSettings::getBridgeIDMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    uint32_t val = preferences.getUInt(SETTINGS_BRIDGE_ID, 0);

    preferences.end();

    return val;
}

void DeviceSettings::setSSID(String SSID){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    WIFISSID = SSID;
    Serial.println("Set SSID = " + SSID);
    preferences.putString(SETTINGS_WIFI_SSID, SSID);

    preferences.end();
}

String DeviceSettings::getSSIDMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_WIFI_SSID, "");

    preferences.end();

    return val;
}

void DeviceSettings::setWIFIPassword(String password){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    WIFIPassword = password;
    preferences.putString(SETTINGS_WIFI_PASSWORD, password);

    preferences.end();
}

String DeviceSettings::getWIFIPasswordMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_WIFI_SSID, "");

    preferences.end();

    return val;
}

void DeviceSettings::setUsername(String username){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    username = username;
    preferences.putString(SETTINGS_SERVER_USERNAME, username);

    preferences.end();
}

String DeviceSettings::getUsernameMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_SERVER_USERNAME, "");

    preferences.end();

    return val;
}

void DeviceSettings::setPassword(String password){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    password = password;
    preferences.putString(SETTINGS_SERVER_PASSWORD, password);

    preferences.end();
}

String DeviceSettings::getPasswordMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_SERVER_PASSWORD, "");

    preferences.end();

    return val;
}

void DeviceSettings::setJWT(String jwt){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    this->jwt = jwt;
    preferences.putString(SETTINGS_JWT, jwt);

    preferences.end();
}

String DeviceSettings::getJWTMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_JWT, "");

    preferences.end();

    return val;
}

void DeviceSettings::setCalibrationVal(float calVal){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    calibrationVal = calVal;
    preferences.putFloat(SETTINGS_CAL_VALUE, calVal);

    preferences.end();
}

float DeviceSettings::getCalibrationValMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    double val = preferences.getFloat(SETTINGS_CAL_VALUE, 1);

    preferences.end();

    return val;
}

void DeviceSettings::setZeroFactor(long zeroFactor){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    this->zeroFactor = zeroFactor;
    preferences.putLong(SETTINGS_ZERO_FACTOR, zeroFactor);

    preferences.end();
}

long DeviceSettings::getZeroFactorMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    long val = preferences.getLong(SETTINGS_ZERO_FACTOR, 0);

    preferences.end();

    return val;
}

void DeviceSettings::setNumItemsPerWeightVal(unsigned int numItems, bool updateServer){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    numItemsPerWeight = numItems;
    preferences.putUInt(SETTINGS_NUM_VALUES_PER, numItems);

    if(updateServer){
        addSettingsItemForMeshToSend(NUM_ITEMS_PER_WEIGHT_KEY, numItems);
    }

    preferences.end();
}

unsigned int DeviceSettings::getNumItemsPerWeightValMem(void){
    // Open namespace for read
    preferences.begin(PREF_NAMESPACE, true);

    unsigned int val = preferences.getUInt(SETTINGS_NUM_VALUES_PER, 1);

    preferences.end();

    return val;
}

void DeviceSettings::setReferenceWeightOfItems(double itemsWeightGrams, bool updateServer){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    referenceWeight = itemsWeightGrams;
    preferences.putDouble(SETTINGS_WEIGHT_REFERENCE, itemsWeightGrams);

    if(updateServer){
        addSettingsItemForMeshToSend(WEIGHT_PER_X_KEY, itemsWeightGrams);
    }

    preferences.end();
}

double DeviceSettings::getReferenceWeightOfItemsGramsMem(void){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, true);

    double val = preferences.getDouble(SETTINGS_WEIGHT_REFERENCE, 10);

    preferences.end();

    return val;
}

void DeviceSettings::saveWeight(double weight){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putDouble(SETTINGS_NUM_WEIGHT, weight);

    preferences.end();
}

double DeviceSettings::getWeightMem(void){
    // Open namespace for read only
    preferences.begin(PREF_NAMESPACE, true);

    double val = preferences.getDouble(SETTINGS_NUM_WEIGHT, 0);

    preferences.end();

    return val;
}

void DeviceSettings::saveMeshName(String name){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putString(SETTINGS_MESH_NAME, name);

    preferences.end();
}

String DeviceSettings::getMeshName(void){
    // Open namespace for read only
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_MESH_NAME, "SCALE");

    preferences.end();

    return val;
}

void DeviceSettings::saveMeshPassword(String password){
    // Open namespace for read and write
    preferences.begin(PREF_NAMESPACE, false);

    preferences.putString(SETTINGS_MESH_PASSWORD, password);

    preferences.end();
}

String DeviceSettings::getMeshPassword(void){
    // Open namespace for read only
    preferences.begin(PREF_NAMESPACE, true);

    String val = preferences.getString(SETTINGS_MESH_PASSWORD, "12345");

    preferences.end();

    return val;
}
