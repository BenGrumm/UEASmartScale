#ifndef BT_SCANNER_H
#define BT_SCANNER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "painlessMesh.h"

void setupBTScan(Scheduler& userScheduler);

void foundDevices(BLEScanResults foundDevices);
void scanDevices(void* args);

#endif