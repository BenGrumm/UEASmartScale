#ifndef BT_SCANNER_H
#define BT_SCANNER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "painlessMesh.h"
#include "mesh_client.hpp"

struct BeaconInfo {
    uint8_t major;
    uint8_t minor;

    double distance;
};

class BT_Scanner{
    public:
        static void setupBTScan(Scheduler& userScheduler);

        static void foundDevices(BLEScanResults foundDevices);
        static void scanDevices(void* args);
    private:
        static int scanTime; //In seconds
        static BLEScan* pBLEScan;
        static BeaconInfo devices[4];
        static uint8_t numFound;
        static uint8_t numScans;
};

#endif