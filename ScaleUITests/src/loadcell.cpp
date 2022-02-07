#include <Arduino.h>
#include "HX711.h"
#include <EEPROM.h>

HX711 scale;

void setupLC(unsigned int dataPin, unsigned int clkPin){
    scale.begin(dataPin, clkPin);
    scale.set_scale();
    scale.tare(); //Reset the scale to 0

    long zero_factor = scale.read_average();
}