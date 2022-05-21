#include "bt_scanner.hpp"

int scanTime = 2; //In seconds
BLEScan* pBLEScan;
BeaconInfo devices[4];
uint8_t numFound = 0;
uint8_t numScans = 0;

void setupBTScan(Scheduler& userScheduler){
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(2000);
    pBLEScan->setWindow(250);  // less or equal setInterval value

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

    for(int i = 0; i < foundDevices.getCount(); i++){
        BLEAdvertisedDevice device = foundDevices.getDevice(i);

        if(device.haveManufacturerData()){
            char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)device.getManufacturerData().data(), device.getManufacturerData().length());

            // Example Data: 4c00021505df65f2b5a111ecb9090242ac12000200010002c5
            String realStringData = String(pHex);
            String UUID = realStringData.substring(8, 40);
            if(UUID.equals("05df65f2b5a111ecb9090242ac120002")){
                String major = realStringData.substring(40, 44);
                uint8_t intMajor = (uint8_t)strtol(major.c_str(), NULL, 16);
                String minor = realStringData.substring(44, 48);
                uint8_t intMinor = (uint8_t)strtol(minor.c_str(), NULL, 16);
                String signalPower = realStringData.substring(48);

                // int8_t sigPow = (int8_t)strtol(signalPower.c_str(), NULL, 16);
                int8_t sigPow = -74;
                double distance = pow(10, (sigPow - device.getRSSI()) / (10 * 3.1));
                Serial.println("Scan Values");
                Serial.printf(
                    "Major: %, Minor: %d, Distance: %f, RSSI: %d, SigPow: %d\n", 
                    intMajor, intMinor, distance, device.getRSSI(), sigPow
                );

                bool existsAndUpdated = false;

                for(int i = 0; i < numFound; i++){
                    if(devices[i].major == intMajor && devices[i].minor == intMinor){
                        devices[i].distance = (devices[i].distance + distance) / 2.0;
                        existsAndUpdated = true;
                        break;
                    }
                }

                if(!existsAndUpdated && numFound <= 3){
                    devices[numFound].major = intMajor;
                    devices[numFound].minor = intMinor;
                    devices[numFound++].distance = distance;
                }else if(!existsAndUpdated){
                    int smallestPos = -1;
                    double deviceVal = distance;
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

    Serial.println("Average:");
    for(int i = 0; i < numFound; i++){
        Serial.printf(
            "Major: %d, Minor: %d, Distance: %f\n", 
            devices[i].major, devices[i].minor, devices[i].distance
        );
    }

    if(numScans++ >= 5 && numFound == 4){
        Serial.println("Adding Beacons");
        addBeacon("beaconOne", devices[0].major, devices[0].minor, devices[0].distance);
        addBeacon("beaconTwo", devices[1].major, devices[1].minor, devices[1].distance);
        addBeacon("beaconThree", devices[2].major, devices[2].minor, devices[2].distance);
        addBeacon("beaconFour", devices[3].major, devices[3].minor, devices[3].distance);
        numScans = 0;
        numFound = 0;
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