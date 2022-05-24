#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <Preferences.h>
#include "mesh_client.hpp"

#define PREF_NAMESPACE "settings"

#define INITIALISATION "DEVICE_INITIALISATION"

#define SETTINGS_WIFI_SSID "WIFI_SSID"
#define SETTINGS_WIFI_PASSWORD "WIFI_PASSWORD"

#define SETTINGS_SERVER_USERNAME "SERVER_USERNAME"
#define SETTINGS_SERVER_PASSWORD "SERVER_PASSWORD"
#define SETTINGS_JWT "JWT"

#define SETTINGS_MESH_NAME "MESH_NAME"
#define SETTINGS_MESH_PASSWORD "MESH_PASSWORD"

#define SETTINGS_NUM_WEIGHT "NUM_WEIGHT"
#define SETTINGS_CAL_VALUE "CAL_VALUE"
#define SETTINGS_ZERO_FACTOR "ZERO_FACTOR"
#define SETTINGS_NUM_VALUES_PER "NUM_VALUES_PER"
#define SETTINGS_WEIGHT_REFERENCE "WEIGHT_REF"

#define SETTINGS_BRIDGE_ID "BRIDGE_ID"

class DeviceSettings{
    public:
        static DeviceSettings* getInstance(){
            static DeviceSettings INSTANCE;
            return &INSTANCE;
        }

        void updateLocalSettings(JsonObject newSettings);
        void setBridgeID(uint32_t id);
        void setSSID(String SSID);
        void setWIFIPassword(String password);
        void setUsername(String username);
        void setPassword(String password);
        void setJWT(String jwt);
        void setCalibrationVal(float calVal);
        void setZeroFactor(long zeroFactor);
        void setNumItemsPerWeightVal(unsigned int numItems, bool updateServer);
        void setReferenceWeightOfItems(double itemsWeightGrams, bool updateServer);
        void saveWeight(double weight);
        void saveMeshName(String name);
        void saveMeshPassword(String password);

        [[nodiscard]] uint32_t getBridgeIDMem(void);
        [[nodiscard]] String getSSIDMem(void);
        [[nodiscard]] String getWIFIPasswordMem(void);
        [[nodiscard]] String getUsernameMem(void);
        [[nodiscard]] String getPasswordMem(void);
        [[nodiscard]] String getJWTMem(void);
        [[nodiscard]] float getCalibrationValMem(void);
        [[nodiscard]] long getZeroFactorMem(void);
        [[nodiscard]] unsigned int getNumItemsPerWeightValMem(void);
        [[nodiscard]] double getReferenceWeightOfItemsGramsMem(void);
        [[nodiscard]] double getWeightMem(void);
        [[nodiscard]] String getMeshName(void);
        [[nodiscard]] String getMeshPassword(void);

        float calibrationVal;
        long zeroFactor;
        unsigned int numItemsPerWeight;
        double referenceWeight;

        String username;
        String password;
        String jwt;

        String WIFISSID;
        String WIFIPassword;

        String meshName;
        String meshPassword;

        uint32_t bridgeID;

        bool initialisation;
    private:
        DeviceSettings();
        Preferences preferences;
};

#endif