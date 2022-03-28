#include "bt_scanner.hpp"

int scanTime = 5; //In seconds
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
    pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // less or equal setInterval value

    pBLEScan->start(scanTime, foundDevices, false);
}

void foundDevices(BLEScanResults foundDevices){
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    Serial.println("Scan done!");
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
}

// void scanDevices(void* args){
//     // put your main code here, to run repeatedly:
//     BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
//     Serial.print("Devices found: ");
//     Serial.println(foundDevices.getCount());
//     Serial.println("Scan done!");
//     pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

//     while(1){
//         vTaskDelay(50000 / portTICK_PERIOD_MS);
//     }
// }