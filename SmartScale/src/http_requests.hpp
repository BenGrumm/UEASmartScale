#ifndef HTTP_REQUESTS_H
#define HTTP_REQUESTS_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "painlessMesh.h"
#include "storage.hpp"
#include "mesh_client.hpp"

class DeviceSettings;

class HTTP_Requests{
    public:
        /**
         * @brief Setup json objects used in http_request functions. Create recurrent task on 
         * other core for making requqests.
         * 
         * @param userScheduler * was switched to rtos function so no longer needed
         */
        static void setupHTTP(Scheduler &userScheduler);

        /**
         * @brief Add all settings from JsonObject to buffer of objects to be uploaded
         * to server in http request
         * 
         * @param scaleSettings Object containing id of node and rest of settings to update
         */
        static void addUpdatedSettings(JsonObject scaleSettings);

        /**
         * @brief Get if the username and password have successfully made a request so user
         * knows if there is a problem
         * 
         * @return true is successful request
         * @return false if either no request was made yet or requests havn't been successful
         */
        static bool hadSuccessfulLogin(void);

        /**
         * @brief When a node received new settings from server and responded with ack
         * add id and settings update to buffer to be sent on next http request
         * 
         * @param id of the node that ackknowledged the updated settings
         */
        static void addSettingsAckID(uint32_t id);
    private:
        const static String SERVER_IP;

        static DynamicJsonDocument returnDoc;
        static DynamicJsonDocument outgoingSettings;

        static JsonObject settingsParent;
        static JsonArray settingsUpdates;

        static HTTPClient http;

        static DeviceSettings* http_local_settings;

        static bool hasAuthed;

        static bool authorise(void);
        static void uploadSettings(void* args);
        static bool updateSettings(void);
        static bool getSettingsToUpdate(void);
};

#endif