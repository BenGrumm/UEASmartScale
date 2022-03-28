#include "http_requests.hpp"

double btcPriceGBP = 0;
DynamicJsonDocument returnDoc(1024);
DynamicJsonDocument outgoingSettings(2048);

JsonObject settingsParent;
JsonArray settingsUpdates;

HTTPClient http;

bool hasAuthed = false;

// Task tryAuth(TASK_SECOND * 10, TASK_FOREVER, &tryConnect);

void setupHTTP(Scheduler &userScheduler){
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
        0                   // Core you want to run the task on (0 or 1) -> (0 gives "Guru Meditation Error: Core  1 panic'ed (Interrupt wdt timeout on CPU1)")
    );
}

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

void uploadSettings(void* args){
    for(;;){ // infinite loop
        // Delay for 30s (like calling the task every 30s freeing up the core)
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        if(WiFi.status()== WL_CONNECTED){
            Serial.println("Connected - Updating");
            updateSettings();
        }else{
            Serial.println("Not Connected");
        }

        Serial.print("WIFI comms running on core ");
        Serial.print(xPortGetCoreID());
        Serial.print(", Heap free = ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");

    }
}

bool updateSettings(void){
    if(settingsUpdates.size() > 0 && deviceSettings.jwt != ""){
        serializeJsonPretty(settingsParent, Serial);
        Serial.println();

        http.begin(SERVER_IP + "/core/settings/");
        http.addHeader("Authorization", "JWT " + deviceSettings.jwt);
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
    }else if(deviceSettings.jwt == ""){
        authorise();
    }

    return false;
}

bool authorise(void){
    if(deviceSettings.username != "" && deviceSettings.password != ""){
        http.begin(SERVER_IP + "/token-auth/");

        // http.addHeader("Authorization", "JWT ");
        http.addHeader("Content-Type", "application/json");

        returnDoc.clear();
        returnDoc["username"] = deviceSettings.username;
        returnDoc["password"] = deviceSettings.password;

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
                setJWT(token);
                hasAuthed = true;
                return true;
            }
        }
    }

    return false;
}

bool hadSuccessfulLogin(void){
    return hasAuthed;
}
