#include "http_requests.hpp"

double btcPriceGBP = 0;
DynamicJsonDocument returnDoc(1024);
DynamicJsonDocument outgoingSettings(1024);
DynamicJsonDocument outgoingWeights(1024);

JsonArray settingsUpdates;
JsonArray itemCountUpdates;

HTTPClient http;

// Task tryAuth(TASK_SECOND * 10, TASK_FOREVER, &tryConnect);

void setupHTTP(Scheduler &userScheduler){
    settingsUpdates = outgoingSettings.createNestedArray("scales");
    itemCountUpdates = outgoingWeights.createNestedArray("itemCountUpdates");

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
        tryConnect,            // Function that should be called
        "tryAuth",          // Name of the task (for debugging)
        3000,               // Stack size (bytes)
        NULL,               // Parameter to pass
        1,                  // Task priority
        NULL,               // Task handle
        0                   // Core you want to run the task on (0 or 1) -> (0 gives "Guru Meditation Error: Core  1 panic'ed (Interrupt wdt timeout on CPU1)")
    );
}

void tryConnect(void* args){
    for(;;){ // infinite loop
        if(WiFi.status()== WL_CONNECTED){
            Serial.println("Start");
            authorise();
            Serial.println("End");
        }else{
            Serial.println("Not Connected");
        }

        Serial.print("http() running on core ");
        Serial.println(xPortGetCoreID());

        // Delay for 10s (like calling the task every 10s freeing up the core)
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
}

bool updateSettings(void){
    if(settingsUpdates.size() > 0){
        http.begin(SERVER_IP + "/core/settings/");
        http.addHeader("Authorization", "JWT " + deviceSettings.jwt);
        http.addHeader("Content-Type", "application/json");

        String output;
        serializeJson(outgoingSettings, output);

        int result = http.POST(output);

        if(result == HTTP_CODE_OK){
            // Success does return the full scale data. Not doing anything with it so
            return true;
        }else if(result == HTTP_CODE_UNAUTHORIZED){
            authorise();
        }

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

void makeRequest(){
  HTTPClient http;
  
  http.begin("https://api.coindesk.com/v1/bpi/currentprice.json");

  int httpCode = http.GET();

  if(httpCode > 0) {
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      deserializeJson(returnDoc, payload);
      btcPriceGBP = returnDoc["bpi"]["GBP"]["rate_float"];
      Serial.printf("BTC Price = $%f\n", btcPriceGBP);
    }
  }
  http.end();
}
