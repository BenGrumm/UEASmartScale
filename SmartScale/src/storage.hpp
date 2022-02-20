#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <Preferences.h>

#define PREF_NAMESPACE "settings"

#define SETTINGS_WIFI_SSID "WIFI_SSID"
#define SETTINGS_WIFI_PASSWORD "WIFI_PASSWORD"

#define SETTINGS_SERVER_USERNAME "SERVER_USERNAME"
#define SETTINGS_SERVER_PASSWORD "SERVER_PASSWORD"
#define SETTINGS_JWT "JWT"

#define SETTINGS_NUM_WEIGHT "NUM_WEIGHT"
#define SETTINGS_CAL_VALUE "CAL_VALUE"
#define SETTINGS_ZERO_FACTOR "ZERO_FACTOR"
#define SETTINGS_NUM_VALUES_PER "NUM_VALUES_PER"
#define SETTINGS_WEIGHT_REFERENCE "WEIGHT_REF"

struct Settings{
    float calibrationVal;
    long zeroFactor;
    unsigned int numItemsPerWeight;
    double referenceWeight;

    String username;
    String password;
    String jwt;

    String WIFISSID;
    String WIFIPassword;
};

extern Settings deviceSettings;

/**
 * @brief Setup function to initialise existing settings
 * 
 */
void setupStorage(void);


// WIFI Login
void setSSID(String SSID);
String getSSIDMem(void);

void setWIFIPassword(String password);
String getWIFIPasswordMem(void);



// Login functions
void setUsername(String username);
String getUsernameMem(void);

void setPassword(String password);
String getPasswordMem(void);

void setJWT(String jwt);
String getJWTMem(void);



// Load cell functions

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
float getCalibrationValMem(void);

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
long getZeroFactorMem(void);

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
unsigned int getNumItemsPerWeightValMem(void);

/**
 * @brief Set the weight of item(s) used for calculating weight of single item along with NumItemsPerWeight
 * 
 * @param itemsWeightGrams weight of the item or items in grams
 */
void setReferenceWeightOfItems(double itemsWeightGrams);
/**
 * @brief Get the weight of item(s) used for calculating weight of single item along with NumItemsPerWeight
 * 
 * @return double weight of the item(s) in grams (if none default = 10)
 */
double getReferenceWeightOfItemsGramsMem(void);

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
double getWeightMem(void);

#endif