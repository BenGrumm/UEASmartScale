#include "http_requests.hpp"

double btcPriceGBP = 0;
DynamicJsonDocument returnDoc(1024);
DynamicJsonDocument outgoingDoc(2048);

JsonArray settings;
JsonArray itemCountUpdates;

HTTPClient http;

// Task tryAuth(TASK_SECOND * 10, TASK_FOREVER, &tryConnect);

void setupHTTP(Scheduler &userScheduler){
    settings = outgoingDoc.createNestedArray("settings");
    itemCountUpdates = outgoingDoc.createNestedArray("itemCountUpdates");

    // Some example for adding objs to array
    
    // JsonObject newObj = settings.createNestedObject();
    // newObj["id"] = 1;
    // // testObj["id"] = 1;
    // // outgoingDoc["settings"].add(testObj);

    // newObj = settings.createNestedObject();
    // newObj["id"] = 159;
    // newObj["weightRefGrams"] = 205;

    // deserializeJson(returnDoc, "{\"id\": 19,  \"weightRefGrams\": 25}");
    // settings.add(returnDoc.as<JsonObject>());

    // serializeJsonPretty(outgoingDoc, Serial);
    // Serial.println();

    xTaskCreatePinnedToCore(
        tryConnect,            // Function that should be called
        "tryAuth",          // Name of the task (for debugging)
        2000,               // Stack size (bytes)
        NULL,               // Parameter to pass
        1,                  // Task priority
        NULL,               // Task handle
        1                   // Core you want to run the task on (0 or 1) -> (0 gives "Guru Meditation Error: Core  1 panic'ed (Interrupt wdt timeout on CPU1)")
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
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

bool authorise(void){
    http.begin(SERVER_IP + "/token-auth/");

    // http.addHeader("Authorization", "JWT ");
    http.addHeader("Content-Type", "application/json");

    returnDoc.clear();
    returnDoc["username"] = "Ben12";
    returnDoc["password"] = "test";

    String output;
    serializeJson(returnDoc, output);

    int result = http.POST(output);

    if(result > 0){
        if(result == HTTP_CODE_OK){
            // Save JWT
            Serial.println(http.getString());
            return true;
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
