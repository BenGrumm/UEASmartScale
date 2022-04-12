#include "bt_scanner.hpp"

int scanTime = 2; //In seconds
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        // https://medium.com/beingcoders/convert-rssi-value-of-the-ble-bluetooth-low-energy-beacons-to-meters-63259f307283#:~:text=Distance%20%3D%2010%20%5E%20((Measured%20Power,RSSI)%2F(10%20*%20N))
        // Measure power of -67 dBm at 1m, 3.5 calculate by finding best fit for rssi measure at 3m
        double distance = pow(10, ((-67-(advertisedDevice.getRSSI()))/(10 * 3.5)));
        Serial.printf("Advertised Device: %s, RSSI = %d, Distance = %f\n", advertisedDevice.toString().c_str(), advertisedDevice.getRSSI(), distance);
    }
};

void setupBTScan(Scheduler& userScheduler){
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(500);
    pBLEScan->setWindow(100);  // less or equal setInterval value

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

    int numFound = 0;
    BeaconInfo devices[3];

    for(int i = 0; i < foundDevices.getCount(); i++){
        BLEAdvertisedDevice device = foundDevices.getDevice(i);
        double rssi = device.getRSSI();

        if(device.haveManufacturerData()){
            char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)device.getManufacturerData().data(), device.getManufacturerData().length());

            // Example Data: 4c00021505df65f2b5a111ecb9090242ac12000200010002c5
            String realStringData = String(pHex);
            String UUID = realStringData.substring(8, 40);
            if(UUID.equals("05df65f2b5a111ecb9090242ac120002")){
                String major = realStringData.substring(40, 44);
                String minor = realStringData.substring(44, 48);
                String signalPower = realStringData.substring(48);

                int8_t sigPow = (int8_t)strtol(signalPower.c_str(), NULL, 16);

                if(numFound <= 2){
                    devices[numFound].major = (uint8_t)strtol(major.c_str(), NULL, 16);
                    devices[numFound].minor = (uint8_t)strtol(minor.c_str(), NULL, 16);
                    devices[numFound++].distance = pow(10, (sigPow - device.getRSSI()) / (10 * 3.5));
                }else{
                    int smallestPos = -1;
                    double deviceVal = pow(10, (sigPow - device.getRSSI()) / (10 * 3.5));
                    double largestDistance = deviceVal;

                    for(int i = 0; i < numFound; i++){
                        if(devices[i].distance > largestDistance){
                            smallestPos = i;
                            largestDistance = devices[i].distance;
                        }
                    }

                    if(smallestPos != -1){
                        devices[smallestPos].major = (uint8_t)strtol(major.c_str(), NULL, 16);
                        devices[smallestPos].minor = (uint8_t)strtol(minor.c_str(), NULL, 16);
                        devices[smallestPos].distance = deviceVal;
                    }
                }
            }
        }
    }

    for(int i = 0; i < numFound; i++){
        Serial.printf(
            "Major: %d, Minor: %d, Distance: %f\n", 
            devices[i].major, devices[i].minor, devices[i].distance
        );
    }

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

        Serial.println("Start Scan.");

        bool ok = pBLEScan->start(scanTime, foundDevices, false);

        vTaskDelay(20000 / portTICK_PERIOD_MS);
    }
}