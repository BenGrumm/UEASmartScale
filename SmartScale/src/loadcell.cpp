#include "loadcell.hpp"

HX711 scale;
unsigned int userKnowWeight = 1000; // Known weight in grams

DeviceSettings* local_loadcell_settings;

Task checkAndUpdateScaleNum(TASK_SECOND * 15, TASK_FOREVER, &checkNumItemsAndUpdate);

/**
 * @brief Setup the communication with HX711 and implement stored settings
 * 
 * @param userScheduler scheduler to make periodic reading of sensor
 * @param dataPin pin to use for data to HX711
 * @param clkPin pin to use for clock to HX711
 */
void setupLC(Scheduler &userScheduler, unsigned int dataPin, unsigned int clkPin){
    local_loadcell_settings = DeviceSettings::getInstance();

    scale.begin(dataPin, clkPin);

    // Retrieve stored vals (if any)
    long zeroFac = local_loadcell_settings->zeroFactor;
    float calVal = local_loadcell_settings->calibrationVal;

    Serial.print("Zero Factor = ");
    Serial.println(zeroFac);
    Serial.print("Calibration Val = ");
    Serial.println(calVal);

    // Configure with stored vals
    scale.set_scale(calVal);
    scale.set_offset(zeroFac); //Used for taring / zeroing

    // Create task for reading values
    userScheduler.addTask(checkAndUpdateScaleNum);
    checkAndUpdateScaleNum.enableDelayed(TASK_SECOND * 10);
}

/**
 * @brief When called will zero the scale with the current weight on it
 * 
 */
void zeroTare(void){
    Serial.println("Zero");
    long avg = scale.read_average();

    local_loadcell_settings->setZeroFactor(avg);

    scale.set_offset(avg);
}

/**
 * @brief Get the weight on the scale in grams
 * 
 * @return double 
 */
double getWeightGrams(void){
    return scale.get_units();
}

/**
 * @brief Read the scale value and store in memory
 * 
 */
void readScale(void){
    local_loadcell_settings->saveWeight(scale.get_units());
}

/**
 * @brief Will calibrate the scale based on weight current on scale and
 *        value stored as the known val
 * 
 */
void calibrateScale(void){
    Serial.println("Calibrate");
    scale.set_scale(1);
    double calibrationFactor = scale.get_units(10) / userKnowWeight;
    scale.set_scale(calibrationFactor);

    local_loadcell_settings->setCalibrationVal(calibrationFactor);
}

/**
 * @brief Set the Known Weight object
 * 
 * @param knownVal value to store
 */
void setKnownWeight(unsigned int knownVal){
    userKnowWeight = knownVal;
}

/**
 * @brief Function to calculate the number of objects on scale from weight in grams and the reference weight and num items stored in memory
 * 
 * @return unsigned int number of items on the scale
 */
double getNumItems(void){
    double ref = local_loadcell_settings->referenceWeight;
    unsigned int numItems = local_loadcell_settings->numItemsPerWeight;
    return round(getWeightGrams() / (ref / numItems));
}

unsigned int lastKnownWeight = 2868;

/**
 * @brief Get the current number of items and if it's changed from last known value, if it has
 * send to the mesh client to be sent to root and updated on the server
 * 
 */
void checkNumItemsAndUpdate(void){
    double currentNum = getNumItems();

    if(currentNum < 0){
        currentNum = 0;
    }

    if(lastKnownWeight != currentNum){
        updateNumStored(currentNum);
        lastKnownWeight = currentNum;
    }
}
