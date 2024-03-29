#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "painlessMesh.h"
#include <math.h>
#include "WiFi.h"

#include "loadcell.hpp"

#ifdef ROOT

#include "settings_server.hpp"

#endif

#define MAX_LOAD_CELL_MASS 50 // The max rated weight for load cell * 10 so fp operations not done in irq functions

// States for UI
#define SLEEP 0
#define HOME 1
#define MENU 2

#define MENU_EXIT 1
#define MENU_SET_MIN 2
#define MENU_SET_WEIGHT 3
#define MENU_SET_NUM 4
#define MENU_NUMBER_SCREEN 5
#define MENU_CALIBRATE_SCALE 6
#define MENU_CALIBRATE_CONFIRM 7
#define MENU_DISPLAY_UNITS 8
#define MENU_SET_WEIGHT_CONFIRM 9
#define MENU_SETTINGS_SERVER 10
#define MENU_SETTINGS_SERVER_CONFIRM 11
#define MENU_SETTINGS_CHECK_ROOT 12
#define MENU_SETTINGS_CHECK_ROOT_CONFIRM 13
#define MENU_SETTINGS_CHECK_NODE 14
#define MENU_SETTINGS_CHECK_NODE_CONFIRM 15
#define MENU_SETTINGS_UPDATE_NAME_PASS 16
#define MENU_SETTINGS_UPDATE_NAME_PASS_CONFIRM 17
#define MENU_SETTINGS_SHOW_ID 18
#define MENU_SETTINGS_SHOW_ID_CONFIRM 19

#define CALIBRATE_ZERO 1
#define CALIBRATE_SET_WEIGHT_PROMPT 2
#define CALIBRATE_SET_WEIGHT 3
#define CALIBRATE_PLACE_WEIGHT 4

#define WEIGHT_SET_CURRENT 1
#define WEIGHT_LIVE_SET 2

#define UPDATE_MESH_NAME 1
#define UPDATE_MESH_PASSWORD 2

/**
 * @brief Function used to setup the UI pins and tasks
 * 
 * @param userScheduler global scheduler called on every loop
 * @param button1Pin pin number for the button 1 input
 * @param button2Pin pin number for the button 2 input
 * @param button3Pin pin number for the button 3 input
 */
void setupUI(Scheduler &userScheduler, int button1Pin, int button2Pin, int button3Pin);

// Non publicly used forward declarations
void drawScreen(void);
void buttonPress_three(void);
void buttonPress_two(void);
void buttonPress_one(void);
void drawMenu(void);
void drawWeightSet(void);
void onePressed(void);
void oneMenuPressed(void);
void twoPressed(void);
void twoMenuPressed(void);
void threePressed(void);
void threeMenuPressed(void);
void drawCalibration(void);
void oneCalibrationPress(void);
void twoCalibrationPress(void);
void threeCalibrationPress(void);
void getLocalNumItemsPerWeightVal(void);
void setStorageNumItemsPerWeightVal(void);
void oneMenuWeightSetPress(void);
void twoMenuWeightSetPress(void);
void threeMenuWeightSetPress(void);
void saveReferenceWeightToStorage(void);
void drawMeshInfoUpdate(void);
void threeMenuMeshInfoUpdate(void);
void twoMenuMeshInfoUpdate(void);
void oneMenuMeshInfoUpdate(void);
void checkButtonStates(void);

#endif