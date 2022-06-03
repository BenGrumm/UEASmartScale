#ifndef LOADCELL_H
#define LOADCELL_H

#include <Arduino.h>
#include <math.h>
#include "HX711.h"
#include "storage.hpp"
#include "painlessMesh.h"

class LoadCell{
    public:
        /**
          * @brief Will setup the communication with HX711 and implement stored settings
          * 
          * @param userScheduler scheduler to make periodic reading of sensor
          * @param dataPin pin to use for data to HX711
          * @param clkPin pin to use for clock to HX711
          */
        static void setupLoadCell(Scheduler &userScheduler, unsigned int dataPin, unsigned int clkPin);

        /**
         * @brief Function to calculate the number of objects on scale from weight in grams and the reference weight and num items stored in memory
         * 
         * @return unsigned int number of items on the scale
         */
        static double getNumItems(void);

        /**
         * @brief When called will zero the scale with the current weight on it
         * 
         */
        static void zeroTare(void);

        /**
         * @brief Get the weight on the scale in grams
         * 
         * @return double 
         */
        static double getWeightGrams(void);

        /**
         * @brief Read the scale value and store in memory
         * 
         */
        static void readScale(void);

        /**
         * @brief Will calibrate the scale based on weight current on scale and
         *        value stored as the known val
         * 
         */
        static void calibrateScale(void);

        /**
         * @brief Set the Known Weight object
         * 
         * @param knownVal value to store
         */
        static void setKnownWeight(unsigned int knownVal);

        /**
         * @brief Get the current number of items and if it's changed from last known value, if it has
         * send to the mesh client to be sent to root and updated on the server
         * 
         */
        static void checkNumItemsAndUpdate(void);

    private:
        static HX711 scale;
        static unsigned int userKnowWeight; // Known weight in grams

        static DeviceSettings* local_loadcell_settings;

        static Task* checkAndUpdateScaleNum;
};

#endif