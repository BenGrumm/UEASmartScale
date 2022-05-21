#include "http_requests.hpp"

double btcPriceGBP = 0;
DynamicJsonDocument returnDoc(1024);
DynamicJsonDocument outgoingSettings(2048);

JsonObject settingsParent;
JsonArray settingsUpdates;

HTTPClient http;

DeviceSettings* http_local_settings;

bool hasAuthed = false;

// Task tryAuth(TASK_SECOND * 10, TASK_FOREVER, &tryConnect);

/**
 * @brief Setup json objects used in http_request functions. Create recurrent task on 
 * other core for making requqests.
 * 
 * @param userScheduler * was switched to rtos function so no longer needed
 */
void setupHTTP(Scheduler &userScheduler){

    http_local_settings = DeviceSettings::getInstance();

    settingsParent = outgoingSettings.createNestedObject();
    settingsUpdates = settingsParent.createNestedArray("scales");
    
    // Some example for adding objs to array
    
    // JsonObject newObj = settingsUpdates.createNestedObject();
    // newObj["id"] = 1;
    // testObj["id"] = 1;
    // outgoingDoc["settings"].add(testObj);

    // newObj = settingsUpdates.createNestedObject();
    // newObj["id"] = 159;
    // newObj["weightRefGrams"] = 205;

    // deserializeJson(returnDoc, "{\"id\": 19,  \"weightRefGrams\": 25}");
    // settingsUpdates.add(returnDoc.as<JsonObject>());

    // settings.clear();

    // serializeJsonPretty(outgoingSettings, Serial);
    // Serial.println();

    xTaskCreatePinnedToCore(
        uploadSettings,     // Function that should be called
        "tryAuth",          // Name of the task (for debugging)
        3000,               // Stack size (bytes)
        NULL,               // Parameter to pass
        1,                  // Task priority
        NULL,               // Task handle
        1                   // Core you want to run the task on (0 or 1) -> (0 gives "Guru Meditation Error: Core  1 panic'ed (Interrupt wdt timeout on CPU1)")
    );
}

/**
 * @brief Add all settings from JsonObject to buffer of objects to be uploaded
 * to server in http request
 * 
 * @param scaleSettings Object containing id of node and rest of settings to update
 */
void addUpdatedSettings(JsonObject scaleSettings){
    for(int i = 0; i < settingsUpdates.size(); i++){
        if(settingsUpdates.getElement(i).containsKey("id") && scaleSettings.containsKey("id") && settingsUpdates.getElement(i)["id"] == scaleSettings["id"]){
            // update the object
            if(scaleSettings.containsKey(NUM_STORED_KEY)){
                settingsUpdates.getElement(i)[NUM_STORED_KEY] = scaleSettings[NUM_STORED_KEY];
            }

            if(scaleSettings.containsKey(WEIGHT_PER_X_KEY)){
                settingsUpdates.getElement(i)[WEIGHT_PER_X_KEY] = scaleSettings[WEIGHT_PER_X_KEY];
            }

            if(scaleSettings.containsKey(NUM_ITEMS_PER_WEIGHT_KEY)){
                settingsUpdates.getElement(i)[NUM_ITEMS_PER_WEIGHT_KEY] = scaleSettings[NUM_ITEMS_PER_WEIGHT_KEY];
            }

            if(scaleSettings.containsKey(MIN_NUM_ITEMS_KEY)){
                settingsUpdates.getElement(i)[MIN_NUM_ITEMS_KEY] = scaleSettings[MIN_NUM_ITEMS_KEY];
            }

            if(scaleSettings.containsKey(OK_NUM_ITEMS_KEY)){
                settingsUpdates.getElement(i)[OK_NUM_ITEMS_KEY] = scaleSettings[OK_NUM_ITEMS_KEY];
            }

            return;
        }
    }

    settingsUpdates.add(scaleSettings); // stores by copy
}

/**
 * @brief Task function that will run every x seconds and make all wifi requests.
 * 
 * @param args all args passed to function (none in this case)
 */
void uploadSettings(void* args){
    for(;;){ // infinite loop
        // Delay for 30s (like calling the task every 30s freeing up the core)
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        if(WiFi.status()== WL_CONNECTED){
            Serial.println("Connected - Updating");
            updateSettings();

            getSettingsToUpdate();
        }else{
            Serial.println("Not Connected");
        }

        // Check on right core
        Serial.print("WIFI comms running on core ");
        Serial.print(xPortGetCoreID());
        Serial.print(", Heap free = ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");

    }
}

/**
 * @brief When a node received new settings from server and responded with ack
 * add id and settings update to buffer to be sent on next http request
 * 
 * @param id of the node that ackknowledged the updated settings
 */
void addSettingsAckID(uint32_t id){
    for(int i = 0; i < settingsUpdates.size(); i++){
        if(settingsUpdates.getElement(i).containsKey("id") && settingsUpdates.getElement(i)["id"] == id){
            settingsUpdates.getElement(i)[UPDATE_SETTINGS_SERVER] = false;
            return;
        }
    }
    
    JsonObject obj = settingsUpdates.createNestedObject();
    obj["id"] = id;
    obj[UPDATE_SETTINGS_SERVER] = false;
}

/**
 * @brief Get the Settings To Update from the http server
 * 
 * @return true if successful request
 * @return false if error in request (usually unauthorised)
 */
bool getSettingsToUpdate(void){
    if(http_local_settings->jwt != ""){
        http.begin(SERVER_IP + "/core/settings/");
        http.addHeader("Authorization", "JWT " + http_local_settings->jwt);
        http.addHeader("Content-Type", "application/json");

        int result = http.GET();

        if(result == HTTP_CODE_OK){
            deserializeJson(returnDoc, http.getString());
            JsonArray jsonArr = returnDoc.as<JsonArray>();

            for(JsonObject obj : jsonArr){
                if(checkIfNodeInNetwork((uint32_t)obj["id"])){
                    // Send this object to node of id
                    sendSettingsToNode(obj);
                }
            }

            return true;
        }else if(result == HTTP_CODE_UNAUTHORIZED){
            authorise();
        }
    }else{
        authorise();
    }
    return false;
}

/**
 * @brief post the settings in buffer to http server
 * 
 * @return true if successful request
 * @return false if error in request (usually unauthorised)
 */
bool updateSettings(void){
    if(settingsUpdates.size() > 0 && http_local_settings->jwt != ""){
        serializeJsonPretty(settingsParent, Serial);
        Serial.println();

        http.begin(SERVER_IP + "/core/settings/");
        http.addHeader("Authorization", "JWT " + http_local_settings->jwt);
        http.addHeader("Content-Type", "application/json");

        String output;
        serializeJson(settingsParent, output);

        int result = http.POST(output);

        if(result == HTTP_CODE_OK){
            // Success does return the full scale data. Not doing anything with it so
            settingsUpdates.clear();
            deserializeJson(returnDoc, http.getString());
            JsonObject obj = returnDoc.as<JsonObject>();

            JsonArray updatedIds = obj["updated"];

            Serial.println("ID's Returned");
            for(uint32_t id : updatedIds){
                Serial.println(id);
                // TODO ack update to every node returned in list
                rootSendUpdateAck(id);
            }

            return true;
        }else if(result == HTTP_CODE_UNAUTHORIZED){
            authorise();
        }
    }else if(http_local_settings->jwt == ""){
        authorise();
    }

    return false;
}

/**
 * @brief Use username and password to get new auth token for making requests
 * 
 * @return true if successful request got token
 * @return false if error in request or invalid username and password
 */
bool authorise(void){
    if(http_local_settings->username != "" && http_local_settings->password != ""){
        http.begin(SERVER_IP + "/token-auth/");

        // http.addHeader("Authorization", "JWT ");
        http.addHeader("Content-Type", "application/json");

        returnDoc.clear();
        returnDoc["username"] = http_local_settings->username;
        returnDoc["password"] = http_local_settings->password;

        String output;
        serializeJson(returnDoc, output);

        int result = http.POST(output);

        if(result > 0){
            if(result == HTTP_CODE_OK){
                // Save JWT
                deserializeJson(returnDoc, http.getString());
                JsonObject obj = returnDoc.as<JsonObject>();
                String token = obj["token"];
                Serial.print("Got JWT From Auth - ");
                Serial.println(token);
                http_local_settings->setJWT(token);
                hasAuthed = true;
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief Get if the username and password have successfully made a request so user
 * knows if there is a problem
 * 
 * @return true is successful request
 * @return false if either no request was made yet or requests havn't been successful
 */
bool hadSuccessfulLogin(void){
    return hasAuthed;
}
