#include "bt_scanner.hpp"

int scanTime = 2; //In seconds
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

void setupBTScan(Scheduler& userScheduler){
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(50);
    pBLEScan->setWindow(49);  // less or equal setInterval value

    // pBLEScan->start(scanTime, foundDevices, false);
    xTaskCreatePinnedToCore(
        scanDevices,     // Function that should be called
        "scanBT",          // Name of the task (for debugging)
        1000,               // Stack size (bytes)
        NULL,               // Parameter to pass
        1,                  // Task priority
        NULL,               // Task handle
        0                   // Core you want to run the task on (0 or 1) -> (0 gives "Guru Meditation Error: Core  1 panic'ed (Interrupt wdt timeout on CPU1)")
    );
}

void foundDevices(BLEScanResults foundDevices){
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    Serial.println("Scan done!");
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
}

void scanDevices(void* args){
    while(1){
        // put your main code here, to run repeatedly:
        // BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
        // Serial.print("Devices found: ");
        // Serial.println(foundDevices.getCount());
        // Serial.println("Scan done!");
        // pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

        bool ok = pBLEScan->start(scanTime, foundDevices, false);

        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}