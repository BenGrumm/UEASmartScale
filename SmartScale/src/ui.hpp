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

class UI{
    public:
        /**
         * @brief Function used to setup the UI pins and tasks
         * 
         * @param userScheduler global scheduler called on every loop
         * @param button1Pin pin number for the button 1 input
         * @param button2Pin pin number for the button 2 input
         * @param button3Pin pin number for the button 3 input
         */
        static void setupUI(Scheduler &userScheduler, int button1Pin, int button2Pin, int button3Pin);

    private:
        static void drawScreen(void);
        static void buttonPress_three(void);
        static void buttonPress_two(void);
        static void buttonPress_one(void);
        static void drawMenu(void);
        static void drawWeightSet(void);
        static void onePressed(void);
        static void oneMenuPressed(void);
        static void twoPressed(void);
        static void twoMenuPressed(void);
        static void threePressed(void);
        static void threeMenuPressed(void);
        static void drawCalibration(void);
        static void oneCalibrationPress(void);
        static void twoCalibrationPress(void);
        static void threeCalibrationPress(void);
        static void getLocalNumItemsPerWeightVal(void);
        static void setStorageNumItemsPerWeightVal(void);
        static void oneMenuWeightSetPress(void);
        static void twoMenuWeightSetPress(void);
        static void threeMenuWeightSetPress(void);
        static void saveReferenceWeightToStorage(void);
        static void drawMeshInfoUpdate(void);
        static void threeMenuMeshInfoUpdate(void);
        static void twoMenuMeshInfoUpdate(void);
        static void oneMenuMeshInfoUpdate(void);
        static void checkButtonStates(void);

        static DeviceSettings* local_ui_settings;

        // vars for debouncing
        static unsigned long button1LastPress;
        static unsigned long button2LastPress;
        static unsigned long button3LastPress;
        static unsigned long debounceDelay;

        static uint8_t button1;
        static uint8_t button2;
        static uint8_t button3;

        static unsigned int screenState; // current lcd state
        static unsigned int menuState;
        static unsigned int calibrateState;
        static unsigned int weightSetState;
        static unsigned int meshUpdateSetState;
        static volatile unsigned int numItemsSet; // temp var for the num items in the MENU_NUMBER_SCREEN state
        static uint8_t updateCursorPos;
        static String meshName;
        static String meshPassword;

        static bool isFirstDraw;
        static bool displayGrams;

        // Custom characters for LCD
        static byte Check[];
        static byte Cross[];

        static Task checkButton;

        static Task drawUI;
        static Task* zeroScale;
        static Task* setKnownVal;
        static Task getLocalNumItem;
        static Task setStorageNumItem;
        static Task setReferenceWeight;

        static LiquidCrystal_I2C lcd;  // set the LCD address to 0x27 for a 16 chars and 2 line display

        // Buffer used for drawing to the LCD (16 chars wide)
        static char buffer[16];
 
        // Vars used in drawing
        static unsigned int numItems;
        static unsigned int lastNumLength;
        static unsigned int numItemsSetLength;
        static unsigned int padding;
        static unsigned int frontPadding;

        static int knownWeight; // Used for calibrating the cell (10x to prevent floating point in irq)
};

#endif