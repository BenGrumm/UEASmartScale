#ifndef BT_SCANNER_H
#define BT_SCANNER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "painlessMesh.h"
#include "mesh_client.hpp"

void setupBTScan(Scheduler& userScheduler);

void foundDevices(BLEScanResults foundDevices);
void scanDevices(void* args);

struct BeaconInfo {
    uint8_t major;
    uint8_t minor;

    double distance;
};

#endif