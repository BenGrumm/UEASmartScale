#include "http_requests.hpp"

double btcPriceGBP = 0;
DynamicJsonDocument returnDoc(1024);
DynamicJsonDocument outgoingSettings(1024);
DynamicJsonDocument outgoingWeights(1024);

JsonObject settingsParent;
JsonArray settingsUpdates;
JsonObject itemCountParent;
JsonArray itemCountUpdates;

HTTPClient http;

// Task tryAuth(TASK_SECOND * 10, TASK_FOREVER, &tryConnect);

void setupHTTP(Scheduler &userScheduler){
    settingsParent = outgoingSettings.createNestedObject();
    settingsUpdates = settingsParent.createNestedArray("scales");
    itemCountParent = outgoingWeights.createNestedObject();
    itemCountUpdates = itemCountParent.createNestedArray("scales");

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
            if(scaleSettings.containsKey("weightGramsPerXItmes")){
                settingsUpdates.getElement(i)["weightGramsPerXItmes"] = scaleSettings["weightGramsPerXItmes"];
            }

            if(scaleSettings.containsKey("numItemsPerWeight")){
                settingsUpdates.getElement(i)["numItemsPerWeight"] = scaleSettings["numItemsPerWeight"];
            }

            if(scaleSettings.containsKey("minNumItems")){
                settingsUpdates.getElement(i)["minNumItems"] = scaleSettings["minNumItems"];
            }

            if(scaleSettings.containsKey("okNumItems")){
                settingsUpdates.getElement(i)["okNumItems"] = scaleSettings["okNumItems"];
            }

            return;
        }
    }

    settingsUpdates.add(scaleSettings); // stores by copy
}

void addUpdatedNumItems(JsonObject scaleNumItems){
    for(int i = 0; i < itemCountUpdates.size(); i++){
        if(itemCountUpdates.getElement(i).containsKey("id") && scaleNumItems.containsKey("id") && itemCountUpdates.getElement(i)["id"] == scaleNumItems["id"]){
            // update the object
            if(scaleNumItems.containsKey(NUM_STORED_KEY)){
                itemCountUpdates.getElement(i)[NUM_STORED_KEY] = scaleNumItems[NUM_STORED_KEY];
            }

            return;
        }
    }

    itemCountUpdates.add(scaleNumItems); // stores by copy
}

void uploadSettings(void* args){
    for(;;){ // infinite loop
        // Delay for 30s (like calling the task every 30s freeing up the core)
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        if(WiFi.status()== WL_CONNECTED){
            Serial.println("Connected - Updating");
            updateSettings();
            updateNumItems();
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

bool updateNumItems(void){
    if(itemCountUpdates.size() > 0 && deviceSettings.jwt != ""){
        serializeJsonPretty(itemCountParent, Serial);
        Serial.println();

        http.begin(SERVER_IP + "/core/scale/");
        http.addHeader("Authorization", "JWT " + deviceSettings.jwt);
        http.addHeader("Content-Type", "application/json");

        String output;
        serializeJson(itemCountParent, output);

        int result = http.POST(output);

        if(result == HTTP_CODE_OK){
            // Success does return the full scale data. Not doing anything with it so
            itemCountUpdates.clear();
            return true;
        }else if(result == HTTP_CODE_UNAUTHORIZED){
            authorise();
        }
    }else if(deviceSettings.jwt == ""){
        authorise();
    }

    return false;
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
                Serial.println(token);
                setJWT(token);
                return true;
            }
        }
    }

    return false;
}
