#include "ui.hpp"

void drawScreen(void);
void buttonPress_three(void);
void buttonPress_two(void);
void buttonPress_one(void);
unsigned int getNumItems(void);

// Ben Female ESP
// #define BUTTON_3    2 // GPIO34
// #define BUTTON_2    0 // GPIO23
// #define BUTTON_1    4 // GPIO22

// Ben Male ESP
#define BUTTON_3    4 // GPIO34
#define BUTTON_2    5 // GPIO23
#define BUTTON_1    15 // GPIO22

#define SLEEP 0
#define HOME 1
#define MENU_EXIT 2
#define MENU_SET_MIN 3
#define MENU_SET_WEIGHT 4
#define MENU_SET_NUM 5
#define MENU_NUMBER_SCREEN 6

unsigned long button1LastPress = 0;
unsigned long button2LastPress = 0;
unsigned long button3LastPress = 0;
unsigned long debounceDelay = 200;
unsigned int screenState = HOME;
unsigned int numItemsSet = 1;

bool isFirstDraw = true;

Task drawUI(TASK_SECOND * 1, TASK_FOREVER, &drawScreen);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setupUI(Scheduler &userScheduler)
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  // Use floating pin value as pseudo random seed
  randomSeed(analogRead(0));

  pinMode(BUTTON_1, INPUT_PULLUP);     // Button Input
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), buttonPress_one, FALLING);
  pinMode(BUTTON_2, INPUT_PULLUP);     // Button Input
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), buttonPress_two, FALLING);
  pinMode(BUTTON_3, INPUT_PULLUP);     // Button Input
  attachInterrupt(digitalPinToInterrupt(BUTTON_3), buttonPress_three, FALLING);

  Serial.println("Setup");

  userScheduler.addTask(drawUI);
  drawUI.enable();
}

char buffer[16];

unsigned int numItems = 359;
unsigned int lastNumLength = 3;
unsigned int numItemsSetLength = 0;
unsigned int padding;
unsigned int frontPadding;

void drawScreen(void){
  Serial.println("Draw");
  if(isFirstDraw){
    lcd.clear();
  }
  switch(screenState){
    case(HOME):
      numItems = getNumItems();
      if(numItems < 10000000000){
        numItemsSetLength = (int)log10(numItems) + 1;
        padding = 16 - (numItemsSetLength + 6); // 16 num position on line - 6 = len(ITEMS) + " "
        frontPadding = (int)(padding / 2);
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
    default:
      // Sleep function
      if(isFirstDraw){
        lcd.noBacklight();
        isFirstDraw = false;
      }
  }
}

void buttonPress_three(){
  if((millis() - button1LastPress) > debounceDelay){
    Serial.println("But 3");
    isFirstDraw = true;
    switch(screenState){
      case(HOME):
        screenState = SLEEP;
        drawUI.forceNextIteration();
        break;
      case(MENU_EXIT):
        screenState = MENU_SET_MIN;
        drawUI.forceNextIteration();
        break;
      case(MENU_SET_MIN):
        screenState = MENU_SET_WEIGHT;
        drawUI.forceNextIteration();
        break;
      case(MENU_SET_WEIGHT):
        screenState = MENU_SET_NUM;
        drawUI.forceNextIteration();
        break;
      case(MENU_SET_NUM):
        screenState = MENU_EXIT;
        drawUI.forceNextIteration();
        break;
      case(MENU_NUMBER_SCREEN):
        Serial.println("Add");
        numItemsSet++;
        break;
      default:
        // Sleep function
        screenState = HOME;
        drawUI.forceNextIteration();
    }
    button1LastPress = millis();
  }
}

void buttonPress_two(){
  if((millis() - button2LastPress) > debounceDelay){
    Serial.println("But 2");
    isFirstDraw = true;
    switch(screenState){
      case(HOME):
        screenState = MENU_EXIT;
        drawUI.forceNextIteration();
        break;
      case(MENU_EXIT):
        screenState = HOME;
        drawUI.forceNextIteration();
        break;
      case(MENU_SET_MIN):
        break;
      case(MENU_SET_WEIGHT):
        break;
      case(MENU_SET_NUM):
        screenState = MENU_NUMBER_SCREEN;
        drawUI.forceNextIteration();
        break;
      case(MENU_NUMBER_SCREEN):
        screenState = MENU_SET_NUM;
        drawUI.forceNextIteration();
        break;
      default:
        // Sleep function
        screenState = HOME;
        drawUI.forceNextIteration();
    }
    button2LastPress = millis();
  }
}

void buttonPress_one(){
  if((millis() - button3LastPress) > debounceDelay){
    Serial.println("But 1");
    isFirstDraw = true;
    switch(screenState){
      case(HOME):
        break;
      case(MENU_EXIT):
        screenState = MENU_SET_NUM;
        drawUI.forceNextIteration();
        break;
      case(MENU_SET_MIN):
        screenState = MENU_EXIT;
        drawUI.forceNextIteration();
        break;
      case(MENU_SET_WEIGHT):
        screenState = MENU_SET_MIN;
        drawUI.forceNextIteration();
        break;
      case(MENU_SET_NUM):
        screenState = MENU_SET_WEIGHT;
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
      default:
        // Sleep function
        screenState = HOME;
        drawUI.forceNextIteration();
    }
    button3LastPress = millis();
  }
}


// TODO replace with proper functions

unsigned int tempNum = 0;

unsigned int getNumItems(void){
  tempNum++;
  if(tempNum < 3){
    return 1;
  }else if(tempNum < 6){
    return 10;
  }else if(tempNum < 9){
    return 10000;
  }else if(tempNum < 12){
    return 100000;
  }else if(tempNum < 15){
    return 1000000;
  }else if(tempNum < 18){
    return 10000000;
  }else if(tempNum < 21){
    return 100000000;
  }else if(tempNum < 24){
    return 1000000000;
  }else if(tempNum < 27){
    return 10000000000;
  }else{
    return random(1, 100000000000);
  }
}