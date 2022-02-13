#include "ui.hpp"
#include "loadcell.hpp"

// Non public forward declarations
void drawScreen(void);
void buttonPress_three(void);
void buttonPress_two(void);
void buttonPress_one(void);
void drawMenu(void);
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

// Temp testing forward declaration
unsigned int getNumItems(void);

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

#define CALIBRATE_ZERO 1
#define CALIBRATE_SET_WEIGHT_PROMPT 2
#define CALIBRATE_SET_WEIGHT 3
#define CALIBRATE_PLACE_WEIGHT 4

// vars for debouncing
unsigned long button1LastPress = 0;
unsigned long button2LastPress = 0;
unsigned long button3LastPress = 0;
unsigned long debounceDelay = 200;

unsigned int screenState = HOME; // current lcd state
unsigned int menuState = MENU_EXIT;
unsigned int calibrateState = CALIBRATE_ZERO;
unsigned int numItemsSet = 1; // temp var for the num items in the MENU_NUMBER_SCREEN state

bool isFirstDraw = true;

Task drawUI(TASK_SECOND * 1, TASK_FOREVER, &drawScreen);
Task zeroScale(TASK_MILLISECOND * 1, TASK_ONCE, &zeroTare);
Task setKnownVal(TASK_MILLISECOND * 1, TASK_ONCE, &calibrateScale);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

/**
 * @brief Function used to setup the UI pins and tasks
 * See header file for more
 */
void setupUI(Scheduler &userScheduler, int button1Pin, int button2Pin, int button3Pin){
    lcd.init();                      // initialize the lcd 
    lcd.backlight();

    // Use floating pin value as pseudo random seed
    randomSeed(analogRead(0));

    // Setup buttons and their interrupts
    pinMode(button1Pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(button1Pin), buttonPress_one, FALLING);
    pinMode(button2Pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(button2Pin), buttonPress_two, FALLING);
    pinMode(button3Pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(button3Pin), buttonPress_three, FALLING);

    // Setup tasks
    userScheduler.addTask(drawUI);
    drawUI.enable();

    userScheduler.addTask(zeroScale); // enabled in interrupt
    userScheduler.addTask(setKnownVal);
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
            numItems = getNumItems();

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
                    lcd.print("ITEMS");
                    lastNumLength = numItemsSetLength;
                }
                lcd.setCursor(frontPadding, 0);
                lcd.print(String(numItems));
            }else{
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
            sprintf(buffer, "%*d", 8, numItemsSet);
            lcd.setCursor(0, 0);
            lcd.print(buffer);
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
    }
}

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
 * @brief Interrupt function for button three
 * 
 */
void buttonPress_three(void){
    if((millis() - button3LastPress) > debounceDelay){
        Serial.println("But 3");
        isFirstDraw = true;
        threePressed();
        button3LastPress = millis();
    }
}

void threePressed(void){
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

void threeMenuPressed(void){
    switch(menuState){
        case(MENU_EXIT):
            menuState = MENU_SET_MIN;
            drawUI.forceNextIteration();
            break;
        case(MENU_SET_MIN):
            menuState = MENU_SET_WEIGHT;
            drawUI.forceNextIteration();
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
            break;
        case(MENU_CALIBRATE_SCALE):
            menuState = MENU_EXIT;
            break;
        case(MENU_CALIBRATE_CONFIRM):
            threeCalibrationPress();
            break;
    }
}

void threeCalibrationPress(void){
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
 * @brief Interrupt function for button two
 * 
 */
void buttonPress_two(void){
    if((millis() - button2LastPress) > debounceDelay){
        Serial.println("But 2");
        isFirstDraw = true;
        twoPressed();
        button2LastPress = millis();
    }
}

void twoPressed(void){
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

void twoMenuPressed(void){
    switch(menuState){
        case(MENU_EXIT):
            screenState = HOME;
            drawUI.forceNextIteration();
            break;
        case(MENU_SET_MIN):
            break;
        case(MENU_SET_WEIGHT):
            break;
        case(MENU_SET_NUM):
            menuState = MENU_NUMBER_SCREEN;
            drawUI.forceNextIteration();
            break;
        case(MENU_NUMBER_SCREEN):
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
    }
}

void twoCalibrationPress(void){
    switch(calibrateState){
        case(CALIBRATE_ZERO):
            calibrateState = CALIBRATE_SET_WEIGHT_PROMPT;
            // Zero now
            zeroScale.enable();
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
            setKnownVal.enable();
            drawUI.forceNextIteration();
            break;
    }
}

/**
 * @brief Interrupt function for button one
 * 
 */
void buttonPress_one(void){
    if((millis() - button3LastPress) > debounceDelay){
        Serial.println("But 1");
        isFirstDraw = true;
        onePressed();
        button3LastPress = millis();
    }
}

void onePressed(void){
    switch(screenState){
        case(HOME):
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

void oneMenuPressed(void){
    switch(menuState){
        case(MENU_EXIT):
            menuState = MENU_CALIBRATE_SCALE;
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
        case(MENU_SET_NUM):
            menuState = MENU_SET_WEIGHT;
            drawUI.forceNextIteration();
            break;
        case(MENU_NUMBER_SCREEN):
            Serial.println("Minus");
            if(numItemsSet > 0){
                numItemsSet--;
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
    }
}   

void oneCalibrationPress(void){
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


// TODO replace with proper functions

unsigned int tempNum = 0;

// unsigned int getNumItems(void){
//     tempNum++;
//     if(tempNum < 3){
//         return 1;
//     }else if(tempNum < 6){
//         return 10;
//     }else if(tempNum < 9){
//         return 10000;
//     }else if(tempNum < 12){
//         return 100000;
//     }else if(tempNum < 15){
//         return 1000000;
//     }else if(tempNum < 18){
//         return 10000000;
//     }else if(tempNum < 21){
//         return 100000000;
//     }else if(tempNum < 24){
//         return 1000000000;
//     }else if(tempNum < 27){
//         return 10000000000;
//     }else{
//         return random(1, 100000000000);
//     }
// }

unsigned int getNumItems(void){
    return (unsigned int) getWeightGrams();
}