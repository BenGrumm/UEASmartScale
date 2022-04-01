#include "ui.hpp"

// vars for debouncing
unsigned long button1LastPress = 0;
unsigned long button2LastPress = 0;
unsigned long button3LastPress = 0;
unsigned long debounceDelay = 200;

uint8_t button1 = 0;
uint8_t button2 = 0;
uint8_t button3 = 0;

unsigned int screenState = HOME; // current lcd state
unsigned int menuState = MENU_EXIT;
unsigned int calibrateState = CALIBRATE_ZERO;
unsigned int weightSetState = WEIGHT_SET_CURRENT;
unsigned int meshUpdateSetState = UPDATE_MESH_NAME;
volatile unsigned int numItemsSet = 1; // temp var for the num items in the MENU_NUMBER_SCREEN state
uint8_t updateCursorPos = 0;
String meshName = "HELLO";
String meshPassword = "12345";

bool isFirstDraw = true;
bool displayGrams = false;

byte Check[] = {
    B00000,
    B00001,
    B00011,
    B10110,
    B11100,
    B01000,
    B00000,
    B00000
};

byte Cross[] = {
    B00000,
    B10001,
    B11011,
    B01110,
    B01110,
    B11011,
    B10001,
    B00000
};

Task checkButton(TASK_SECOND / 500, TASK_FOREVER, &checkButtonStates);

Task drawUI(TASK_SECOND * 1, TASK_FOREVER, &drawScreen);
Task zeroScale(TASK_IMMEDIATE, TASK_ONCE, &zeroTare);
Task setKnownVal(TASK_IMMEDIATE, TASK_ONCE, &calibrateScale);
Task getLocalNumItem(TASK_IMMEDIATE, TASK_ONCE, &getLocalNumItemsPerWeightVal);
Task setStorageNumItem(TASK_IMMEDIATE, TASK_ONCE, &setStorageNumItemsPerWeightVal);
Task setReferenceWeight(TASK_IMMEDIATE, TASK_ONCE, &saveReferenceWeightToStorage);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

/**
 * @brief Function used to setup the UI pins and tasks
 * See header file for more
 */
void setupUI(Scheduler &userScheduler, int button1Pin, int button2Pin, int button3Pin){
    lcd.init();                      // initialize the lcd 
    lcd.backlight();

    lcd.createChar(0, Check);
    lcd.createChar(1, Cross);

    // Use floating pin value as pseudo random seed (used in testing)
    // randomSeed(analogRead(0));

    button1 = button1Pin;
    button2 = button2Pin;
    button3 = button3Pin;

    // Setup buttons and their interrupts
    pinMode(button1Pin, INPUT_PULLUP);
    pinMode(button2Pin, INPUT_PULLUP);
    pinMode(button3Pin, INPUT_PULLUP);

    // attachInterrupt(digitalPinToInterrupt(button1Pin), buttonPress_one, FALLING);
    // attachInterrupt(digitalPinToInterrupt(button2Pin), buttonPress_two, FALLING);
    // attachInterrupt(digitalPinToInterrupt(button3Pin), buttonPress_three, FALLING);

    // Setup tasks
    Serial.println("Add");
    userScheduler.addTask(drawUI);
    userScheduler.addTask(checkButton);
    drawUI.enable();
    checkButton.enable();
    Serial.println("Done");

    userScheduler.addTask(zeroScale); // enabled in interrupt
    userScheduler.addTask(setKnownVal);
    userScheduler.addTask(getLocalNumItem);
    userScheduler.addTask(setStorageNumItem);
    userScheduler.addTask(setReferenceWeight);

}

/**
 * @brief For non interrupt buttons
 * 
 */
void checkButtonStates(void){
    if(digitalRead(button1) == LOW){
        buttonPress_one();
    }

    if(digitalRead(button2) == LOW){
        buttonPress_two();
    }

    if(digitalRead(button3) == LOW){
        buttonPress_three();
    }
}

// Buffer used for drawing to the LCD (16 chars wide)
char buffer[16];

// Vars used in drawing
unsigned int numItems = 359;
unsigned int lastNumLength = 3;
unsigned int numItemsSetLength = 0;
unsigned int padding;
unsigned int frontPadding;

int knownWeight = 15; // Used for calibrating the cell (10x to prevent floating point in irq)

/**
 * @brief Main draw screen function called by task scheduler periodically
 */
void drawScreen(void){
    Serial.println("Draw");
    if(isFirstDraw){
        lcd.clear();
    }
    switch(screenState){
        case(HOME):
            if(displayGrams){
                numItems = getWeightGrams();
            }else{
                numItems = getNumItems();
            }

            if(numItems < 0){
                numItems = 0;
            }

            // Write number first as this uses isFirstDraw var
            if(numItems < 10000000000){
                // Find the number of chars in number as to properly centre the numbers
                numItemsSetLength = (int)log10(numItems) + 1;
                padding = 16 - (numItemsSetLength + 6); // 16 num position on line - 6 = len(ITEMS) + " "
                frontPadding = (int)(padding / 2);

                // Only redraw the padding and items when the length of the numbers is different
                if(numItemsSetLength != lastNumLength || isFirstDraw){
                    lcd.setCursor(0, 0);
                    lcd.print("                ");
                    lcd.setCursor(frontPadding + numItemsSetLength + 1, 0); // add 1 for the extra space between numbers and "ITEMS"
                    if(displayGrams){
                        lcd.print("GRAMS");
                    }else{
                        lcd.print("ITEMS");
                    }
                    lastNumLength = numItemsSetLength;
                }
                // Draw updated number every time
                lcd.setCursor(frontPadding, 0);
                lcd.print(String(numItems));
            }else{
                // 10000000000 should be grams or num this is too many
                lcd.setCursor(0, 0);
                lcd.print("      ERROR      ");
            }

            if(isFirstDraw){
                lcd.backlight();
                lcd.setCursor(0, 1);
                lcd.print("zero  menu  disp");
                isFirstDraw = false;
            }
            break;
        case(MENU):
            drawMenu();
            break;
        default:
            // Sleep function
            if(isFirstDraw){
                lcd.noBacklight();
                isFirstDraw = false;
            }
    }
}

/**
 * @brief Function to draw the current menu state to lcd
 * 
 */
void drawMenu(void){
    switch(menuState){
        case(MENU_EXIT):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("      EXIT      ");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        case(MENU_SET_MIN):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("  SET MIN LEVL  ");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        case(MENU_SET_WEIGHT):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("  SET  WEIGHT  ");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        case(MENU_SET_WEIGHT_CONFIRM):
            drawWeightSet();
            break;
        case(MENU_SET_NUM):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print(" SET  NUM  PER ");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        case(MENU_NUMBER_SCREEN):
            if(isFirstDraw){
                lcd.setCursor(0, 1);
                lcd.print("-1     ok     +1");
                isFirstDraw = false;
            }
            numItemsSetLength = (int)log10(numItemsSet) + 1;
            lcd.setCursor((int)(16 - numItemsSetLength) / 2, 0);
            Serial.println(numItemsSet);
            lcd.print(numItemsSet);
            break;
        case(MENU_CALIBRATE_SCALE):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("CALIBRATE WEIGHT");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        case(MENU_CALIBRATE_CONFIRM):
            drawCalibration();
            break;
        case(MENU_DISPLAY_UNITS):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                if(displayGrams){
                    lcd.print("  DISPLAY  NUM  ");
                }else{
                    lcd.print(" DISPLAY  GRAMS ");
                }
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        
        #ifdef ROOT

        case(MENU_SETTINGS_SERVER):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("Settings  Server");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;

        case(MENU_SETTINGS_SERVER_CONFIRM):
            if(isFirstDraw){
                httpServerOn();
                String ipAp = getAPIPStr();
                lcd.setCursor((int)(16 - ipAp.length()) / 2, 0);
                lcd.print(ipAp);
                lcd.setCursor(0, 1);
                lcd.print("      stop      ");
                isFirstDraw = false;
            }
            break;

        case(MENU_SETTINGS_CHECK_ROOT):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("Check R Settings");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        case(MENU_SETTINGS_CHECK_ROOT_CONFIRM):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print(" WIFI ");
                lcd.setCursor(8, 0);
                lcd.print("Login ");
                lcd.setCursor(0, 1);
                lcd.print("      exit    ");
                isFirstDraw = false;
            }

            if(WiFi.status()== WL_CONNECTED){
                lcd.setCursor(6, 0);
                lcd.write(byte(0));
            }else{
                lcd.setCursor(6, 0);
                lcd.write(byte(1));
            }

            if(hadSuccessfulLogin()){
                lcd.setCursor(14, 0);
                lcd.write(byte(0));
            }else{
                lcd.setCursor(14, 0);
                lcd.write(byte(1));
            }

            break;
        #endif

        case(MENU_SETTINGS_CHECK_NODE):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("Check N Settings");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        case(MENU_SETTINGS_CHECK_NODE_CONFIRM):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("Root ");
                lcd.setCursor(7, 0);
                lcd.print("Num ");
                lcd.setCursor(0, 1);
                lcd.print("      exit    ");
                isFirstDraw = false;
            }

            if(getIfBridgeExists()){
                lcd.setCursor(5, 0);
                lcd.write(byte(0));
            }else{
                lcd.setCursor(5, 0);
                lcd.write(byte(1));
            }

            lcd.setCursor(11, 0);
            lcd.print(getNumConnectedNodes());

            break;
        case(MENU_SETTINGS_UPDATE_NAME_PASS):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("Update Mesh Info");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        case(MENU_SETTINGS_UPDATE_NAME_PASS_CONFIRM):
            drawMeshInfoUpdate();
            break;
        case(MENU_SETTINGS_SHOW_ID):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("  Show Mesh ID  ");
                lcd.setCursor(0, 1);
                lcd.print("<   continue   >");
                isFirstDraw = false;
            }
            break;
        case(MENU_SETTINGS_SHOW_ID_CONFIRM):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("   ");
                lcd.setCursor(3, 0);
                lcd.print(getMeshID());
                lcd.setCursor(0, 1);
                lcd.print("      exit     ");
                isFirstDraw = false;
            }
            break;
    }
}

/**
 * @brief Draw the calibration menu base on current state
 * 
 */
void drawCalibration(void){
    switch(calibrateState){
        case(CALIBRATE_ZERO):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print(" Remove All Mass ");
                lcd.setCursor(0, 1);
                lcd.print("    continue    ");
                isFirstDraw = false;
            }
            break;
        case(CALIBRATE_SET_WEIGHT_PROMPT):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print(" Set Your Mass  ");
                lcd.setCursor(0, 1);
                lcd.print("    continue    ");
                isFirstDraw = false;
            }
            break;
        case(CALIBRATE_SET_WEIGHT):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("     1.0 kg     ");
                lcd.setCursor(0, 1);
                lcd.print("-.5 continue +.5");
                isFirstDraw = false;
            }

            lcd.setCursor(5, 0);
            lcd.print(String((knownWeight / 10.0), 1));

            break;
        case(CALIBRATE_PLACE_WEIGHT):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("   Place Mass   ");
                lcd.setCursor(0, 1);
                lcd.print("    continue    ");
                isFirstDraw = false;
            }
            break;
    }
}

/**
 * @brief Draw the screen when in the weight set confirm state
 * 
 */
void drawWeightSet(void){
    switch(weightSetState){
        case(WEIGHT_SET_CURRENT):
            if(isFirstDraw){
                double currentStored = deviceSettings.referenceWeight;
                // double currentStored = 10000000000000;
                int len = (int)log10(currentStored) + 1;
                if(len > 6 && len < 17){
                    sprintf(buffer, "%.0fg", currentStored);
                    lcd.setCursor((int)(16 - len) / 2, 0);
                    lcd.print(buffer);
                }else if(len < 17){
                    // 16 chars on line 9 taken up by current g and a single " "
                    // Gives padding at start for lcd
                    int space = (int)(16 - 9 - len) / 2;
                    lcd.setCursor(space, 0);
                    sprintf(buffer, "Current %.0fg", currentStored);
                    lcd.print(buffer);
                }
                lcd.setCursor(0, 1);
                lcd.print("cancl      updte");
                isFirstDraw = false;
            }
            break;
        case(WEIGHT_LIVE_SET):
            if(isFirstDraw){
                lcd.setCursor(0, 1);
                lcd.print("      set       ");
                isFirstDraw = false;
            }

            double weightGrams = getWeightGrams();
            int len = (int)log10(weightGrams) + 1;
            if(len >= 15){
                // ERROR Handling
                lcd.setCursor(0, 0);
                lcd.print("    ERR grams   ");
            }else{

                // Clear the row
                lcd.setCursor(0, 0);
                lcd.print("                ");
                // Print current weight
                lcd.setCursor((int)(16 - len - 1) / 2, 0);
                sprintf(buffer, "%.0fg", weightGrams);
                lcd.print(buffer);
            }
            break;
    }
}

/**
 * @brief 
 * 
 */
void IRAM_ATTR drawMeshInfoUpdate(void){
    switch(meshUpdateSetState){
        case(UPDATE_MESH_NAME):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("      " + meshName);
                lcd.setCursor(0, 1);
                lcd.print("-      set     +");
                isFirstDraw = false;
                lcd.setCursor(6 + updateCursorPos, 0);
                lcd.blink_on();
                lcd.cursor_on();
            }
            break;
        case(UPDATE_MESH_PASSWORD):
            if(isFirstDraw){
                lcd.setCursor(0, 0);
                lcd.print("      " + meshPassword);
                lcd.setCursor(0, 1);
                lcd.print("-      set     +");
                isFirstDraw = false;
                lcd.setCursor(6 + updateCursorPos, 0);
                lcd.blink_on();
                lcd.cursor_on();
            }
            break;
    }
}

/**
 * @brief Interrupt function for button three
 * 
 */
void IRAM_ATTR buttonPress_three(void){
    if((millis() - button3LastPress) > debounceDelay){
        Serial.println("But 3");
        isFirstDraw = true;
        threePressed();
        button3LastPress = millis();
    }
}

/**
 * @brief Function called when the third button is pressed
 * 
 */
void IRAM_ATTR threePressed(void){
    switch(screenState){
        case(HOME):
            screenState = SLEEP;
            drawUI.forceNextIteration();
            break;
        case(MENU):
            threeMenuPressed();
            break;
        default:
            // Sleep function
            screenState = HOME;
            drawUI.forceNextIteration();
    }
}

/**
 * @brief Function called when third button is pressed and the UI is in the menu state
 * 
 */
void IRAM_ATTR threeMenuPressed(void){
    switch(menuState){
        case(MENU_EXIT):
            menuState = MENU_SET_MIN;
            drawUI.forceNextIteration();
            break;
        case(MENU_SET_MIN):
            menuState = MENU_SET_WEIGHT;
            drawUI.forceNextIteration();
            break;
        case(MENU_SET_WEIGHT_CONFIRM):
            threeMenuWeightSetPress();
            break;
        case(MENU_SET_WEIGHT):
            menuState = MENU_SET_NUM;
            drawUI.forceNextIteration();
            break;
        case(MENU_SET_NUM):
            menuState = MENU_CALIBRATE_SCALE;
            drawUI.forceNextIteration();
            break;
        case(MENU_NUMBER_SCREEN):
            Serial.println("Add");
            numItemsSet++;
            drawUI.forceNextIteration();
            break;
        case(MENU_CALIBRATE_SCALE):
            menuState = MENU_DISPLAY_UNITS;
            break;
        case(MENU_CALIBRATE_CONFIRM):
            threeCalibrationPress();
            break;
        case(MENU_DISPLAY_UNITS):
            #ifdef ROOT
            menuState = MENU_SETTINGS_SERVER;
            #else
            menuState = MENU_SETTINGS_CHECK_NODE;
            #endif
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_SERVER):
            menuState = MENU_SETTINGS_CHECK_ROOT;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_SERVER_CONFIRM):
            break;
        case(MENU_SETTINGS_CHECK_ROOT):
            menuState = MENU_SETTINGS_CHECK_NODE;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_CHECK_NODE):
            menuState = MENU_SETTINGS_UPDATE_NAME_PASS;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_UPDATE_NAME_PASS):
            menuState = MENU_SETTINGS_SHOW_ID;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_UPDATE_NAME_PASS_CONFIRM):
            threeMenuMeshInfoUpdate();
            break;
        case(MENU_SETTINGS_SHOW_ID):
            menuState = MENU_EXIT;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_SHOW_ID_CONFIRM):
            break;
    }
}

/**
 * @brief Function called on button press 3 when the UI is in the calibration state
 * 
 */
void IRAM_ATTR threeCalibrationPress(void){
    switch(calibrateState){
        case(CALIBRATE_ZERO):
            break;
        case(CALIBRATE_SET_WEIGHT_PROMPT):
            break;
        case(CALIBRATE_SET_WEIGHT):
            if(knownWeight < (MAX_LOAD_CELL_MASS - 5)){
                knownWeight += 5;
                drawUI.forceNextIteration();
            }
            break;
        case(CALIBRATE_PLACE_WEIGHT):
            break;
    }
}

/**
 * @brief Function called on button press 3 when UI is in Weight Menu Press
 * 
 */
void IRAM_ATTR threeMenuWeightSetPress(void){
    switch(weightSetState){
        case(WEIGHT_SET_CURRENT):
            // Confirm want to update
            weightSetState = WEIGHT_LIVE_SET;
            drawUI.forceNextIteration();
            break;
        case(WEIGHT_LIVE_SET):
            // Do nothing
            break;
    }
}

/**
 * @brief 
 * 
 */
void IRAM_ATTR threeMenuMeshInfoUpdate(void){
    switch(meshUpdateSetState){
        case(UPDATE_MESH_NAME):
            meshName[updateCursorPos]++;

            if(meshName[updateCursorPos] == ('Z' + 1)){
                meshName[updateCursorPos] = '0';
            }else if(meshName[updateCursorPos] == ('9' + 1)){
                meshName[updateCursorPos] = 'A';
            }
            break;
        case(UPDATE_MESH_PASSWORD):
            meshPassword[updateCursorPos]++;

            if(meshPassword[updateCursorPos] == ('Z' + 1)){
                meshPassword[updateCursorPos] = '0';
            }else if(meshPassword[updateCursorPos] == ('9' + 1)){
                meshPassword[updateCursorPos] = 'A';
            }
            break;
    }

    drawUI.forceNextIteration();
}

/**
 * @brief Interrupt function for button two
 * 
 */
void IRAM_ATTR buttonPress_two(void){
    if((millis() - button2LastPress) > debounceDelay){
        Serial.println("But 2");
        isFirstDraw = true;
        twoPressed();
        button2LastPress = millis();
    }
}

/**
 * @brief Function called when the second button is pressed
 * 
 */
void IRAM_ATTR twoPressed(void){
    switch(screenState){
        case(HOME):
            screenState = MENU;
            drawUI.forceNextIteration();
            break;
        case(MENU):
            twoMenuPressed();
            break;
        default:
            // Sleep function
            screenState = HOME;
            drawUI.forceNextIteration();
    }
}

/**
 * @brief Function called when second button is pressed and the UI is in the menu state
 * 
 */
void IRAM_ATTR twoMenuPressed(void){
    switch(menuState){
        case(MENU_EXIT):
            screenState = HOME;
            drawUI.forceNextIteration();
            break;
        case(MENU_SET_MIN):
            break;
        case(MENU_SET_WEIGHT):
            menuState = MENU_SET_WEIGHT_CONFIRM;
            break;
        case(MENU_SET_WEIGHT_CONFIRM):
            twoMenuWeightSetPress();
            break;
        case(MENU_SET_NUM):
            getLocalNumItem.setIterations(TASK_ONCE);
            getLocalNumItem.enable();
            menuState = MENU_NUMBER_SCREEN;
            drawUI.forceNextIteration();
            break;
        case(MENU_NUMBER_SCREEN):
            setStorageNumItem.setIterations(TASK_ONCE);
            setStorageNumItem.enable();
            menuState = MENU_SET_NUM;
            drawUI.forceNextIteration();
            break;
        case(MENU_CALIBRATE_SCALE):
            menuState = MENU_CALIBRATE_CONFIRM;
            drawUI.forceNextIteration();
            break;
        case(MENU_CALIBRATE_CONFIRM):
            twoCalibrationPress();
            break;
        case(MENU_DISPLAY_UNITS):
            displayGrams = !displayGrams;
            drawUI.forceNextIteration();
            break;
        
        #ifdef ROOT

        case(MENU_SETTINGS_SERVER):
            menuState = MENU_SETTINGS_SERVER_CONFIRM;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_SERVER_CONFIRM):
            httpServerOff();
            menuState = MENU_SETTINGS_SERVER;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_CHECK_ROOT):
            menuState = MENU_SETTINGS_CHECK_ROOT_CONFIRM;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_CHECK_ROOT_CONFIRM):
            menuState = MENU_SETTINGS_CHECK_ROOT;
            drawUI.forceNextIteration();
            break;
        #endif
        case(MENU_SETTINGS_CHECK_NODE):
            menuState = MENU_SETTINGS_CHECK_NODE_CONFIRM;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_CHECK_NODE_CONFIRM):
            menuState = MENU_SETTINGS_CHECK_NODE;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_UPDATE_NAME_PASS):
            menuState = MENU_SETTINGS_UPDATE_NAME_PASS_CONFIRM;
            meshName = deviceSettings.meshName;
            meshPassword = deviceSettings.meshPassword;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_UPDATE_NAME_PASS_CONFIRM):
            twoMenuMeshInfoUpdate();
            break;
        case(MENU_SETTINGS_SHOW_ID):
            menuState = MENU_SETTINGS_SHOW_ID_CONFIRM;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_SHOW_ID_CONFIRM):
            menuState = MENU_SETTINGS_SHOW_ID;
            drawUI.forceNextIteration();
            break;
    }
}

/**
 * @brief Function called on button press 2 when the UI is in the calibration state
 * 
 */
void IRAM_ATTR twoCalibrationPress(void){
    switch(calibrateState){
        case(CALIBRATE_ZERO):
            calibrateState = CALIBRATE_SET_WEIGHT_PROMPT;
            // Zero now
            zeroScale.setIterations(TASK_ONCE);
            zeroScale.enableIfNot();
            drawUI.forceNextIteration();
            break;
        case(CALIBRATE_SET_WEIGHT_PROMPT):
            calibrateState = CALIBRATE_SET_WEIGHT;
            drawUI.forceNextIteration();
            break;
        case(CALIBRATE_SET_WEIGHT):
            calibrateState = CALIBRATE_PLACE_WEIGHT;
            setKnownWeight(knownWeight * 100);
            drawUI.forceNextIteration();
            break;
        case(CALIBRATE_PLACE_WEIGHT):
            calibrateState = CALIBRATE_ZERO;
            menuState = MENU_CALIBRATE_SCALE;
            // Calculate val
            setKnownVal.setIterations(TASK_ONCE);
            setKnownVal.enableIfNot();
            drawUI.forceNextIteration();
            break;
    }
}

/**
 * @brief Function called on button press 2 when UI is in Weight Menu Press
 * 
 */
void IRAM_ATTR twoMenuWeightSetPress(void){
    switch(weightSetState){
        case(WEIGHT_SET_CURRENT):
            // Do nothing
            break;
        case(WEIGHT_LIVE_SET):
            // Use current weight and save
            weightSetState = WEIGHT_SET_CURRENT;
            menuState = MENU_SET_WEIGHT;
            // enable task
            setReferenceWeight.setIterations(TASK_ONCE);
            setReferenceWeight.enableIfNot();
            drawUI.forceNextIteration();
            break;
    }
}

/**
 * @brief 
 * 
 */
void IRAM_ATTR twoMenuMeshInfoUpdate(void){
    updateCursorPos++;

    switch(meshUpdateSetState){
        case(UPDATE_MESH_NAME):
            if(updateCursorPos > MESH_NAME_SIZE - 1){
                meshUpdateSetState = UPDATE_MESH_PASSWORD;
                updateCursorPos = 0;
            }
            break;
        case(UPDATE_MESH_PASSWORD):
            if(updateCursorPos > MESH_NAME_SIZE - 1){
                meshUpdateSetState = UPDATE_MESH_NAME;
                menuState = MENU_SETTINGS_UPDATE_NAME_PASS;
                updateCursorPos = 0;
                lcd.blink_off();
                lcd.cursor_off();
                Serial.println("Before = " + deviceSettings.meshName + ", " + deviceSettings.meshPassword + " - After = " + meshName + ", " + meshPassword);
                bool restart = false;
                if(deviceSettings.meshName != meshName){
                    saveMeshName(meshName);
                    restart = true;
                }

                if(deviceSettings.meshPassword != meshPassword){
                    saveMeshPassword(meshPassword);
                    restart = true;
                }

                if(restart){
                    ESP.restart();
                }
            }
            break;
    }
    drawUI.forceNextIteration();
}

/**
 * @brief Interrupt function for button one
 * 
 */
void IRAM_ATTR buttonPress_one(void){
    if((millis() - button3LastPress) > debounceDelay){
        Serial.println("But 1");
        isFirstDraw = true;
        onePressed();
        button3LastPress = millis();
    }
}

/**
 * @brief Function called when the first button is pressed
 * 
 */
void IRAM_ATTR onePressed(void){
    switch(screenState){
        case(HOME):
            // zero scale
            zeroScale.setIterations(TASK_ONCE);
            zeroScale.enableIfNot();
            break;
        case(MENU):
            oneMenuPressed();
            break;
        default:
            // Sleep function
            screenState = HOME;
            drawUI.forceNextIteration();
    }
}

/**
 * @brief Function called when first button is pressed and the UI is in the menu state
 * 
 */
void IRAM_ATTR oneMenuPressed(void){
    switch(menuState){
        case(MENU_EXIT):
            menuState = MENU_SETTINGS_SHOW_ID;
            drawUI.forceNextIteration();
            break;
        case(MENU_SET_MIN):
            menuState = MENU_EXIT;
            drawUI.forceNextIteration();
            break;
        case(MENU_SET_WEIGHT):
            menuState = MENU_SET_MIN;
            drawUI.forceNextIteration();
            break;
        case(MENU_SET_WEIGHT_CONFIRM):
            oneMenuWeightSetPress();
            break;
        case(MENU_SET_NUM):
            menuState = MENU_SET_WEIGHT;
            drawUI.forceNextIteration();
            break;
        case(MENU_NUMBER_SCREEN):
            Serial.println("Minus");
            if(numItemsSet > 1){
                numItemsSet--;
                Serial.println(numItemsSet);
                drawUI.forceNextIteration();
            }
            drawUI.forceNextIteration();
            break;
        case(MENU_CALIBRATE_SCALE):
            menuState = MENU_SET_NUM;
            drawUI.forceNextIteration();
            break;
        case(MENU_CALIBRATE_CONFIRM):
            oneCalibrationPress();
            break;
        case(MENU_DISPLAY_UNITS):
            menuState = MENU_CALIBRATE_SCALE;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_SERVER):
            menuState = MENU_DISPLAY_UNITS;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_SERVER_CONFIRM):
            break;
        case(MENU_SETTINGS_CHECK_ROOT):
            menuState = MENU_SETTINGS_SERVER;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_CHECK_NODE):
            #ifdef ROOT
            menuState = MENU_SETTINGS_CHECK_ROOT;
            #else
            menuState = MENU_DISPLAY_UNITS;
            #endif
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_UPDATE_NAME_PASS):
            menuState = MENU_SETTINGS_CHECK_NODE;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_UPDATE_NAME_PASS_CONFIRM):
            oneMenuMeshInfoUpdate();
            break;
        case(MENU_SETTINGS_SHOW_ID):
            menuState = MENU_SETTINGS_UPDATE_NAME_PASS;
            drawUI.forceNextIteration();
            break;
        case(MENU_SETTINGS_SHOW_ID_CONFIRM):
            break;
    }
}

/**
 * @brief Function called on button press 1 when the UI is in the calibration state
 * 
 */
void IRAM_ATTR oneCalibrationPress(void){
    switch(calibrateState){
        case(CALIBRATE_ZERO):
            break;
        case(CALIBRATE_SET_WEIGHT_PROMPT):
            break;
        case(CALIBRATE_SET_WEIGHT):
            if(knownWeight > 5){
                knownWeight -= 5;
                drawUI.forceNextIteration();
            }
            break;
        case(CALIBRATE_PLACE_WEIGHT):
            break;
    }
}

/**
 * @brief Function called on button press 1 when UI is in Weight Menu Press
 * 
 */
void IRAM_ATTR oneMenuWeightSetPress(void){
    switch(weightSetState){
        case(WEIGHT_SET_CURRENT):
            // Cancel and return
            menuState = MENU_SET_WEIGHT;
            drawUI.forceNextIteration();
            break;
        case(WEIGHT_LIVE_SET):
            // Do nothing
            break;
    }
}

/**
 * @brief 
 * 
 */
void IRAM_ATTR oneMenuMeshInfoUpdate(void){
    switch(meshUpdateSetState){
        case(UPDATE_MESH_NAME):
            meshName[updateCursorPos]--;

            if(meshName[updateCursorPos] == ('A' - 1)){
                meshName[updateCursorPos] = '9';
            }else if(meshName[updateCursorPos] == ('0' - 1)){
                meshName[updateCursorPos] = 'Z';
            }
            break;
        case(UPDATE_MESH_PASSWORD):
            meshPassword[updateCursorPos]--;

            if(meshPassword[updateCursorPos] == ('A' - 1)){
                meshPassword[updateCursorPos] = '9';
            }else if(meshPassword[updateCursorPos] == ('0' - 1)){
                meshPassword[updateCursorPos] = 'Z';
            }
            break;
    }
    drawUI.forceNextIteration();
}

/**
 * @brief Retieve stored value of numItemsSet from the eeprom
 * 
 */
void getLocalNumItemsPerWeightVal(void){
    numItemsSet = deviceSettings.numItemsPerWeight;
}

/**
 * @brief Store the value of numItemsSet in the eeprom
 * 
 */
void setStorageNumItemsPerWeightVal(void){
    setNumItemsPerWeightVal(numItemsSet, true);
}

/**
 * @brief Save the current weight as the stored value
 * 
 */
void saveReferenceWeightToStorage(void){
    setReferenceWeightOfItems(getWeightGrams(), true);
}

