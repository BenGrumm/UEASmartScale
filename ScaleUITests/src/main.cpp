#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TaskScheduler.h>
#include <math.h>

#define BUTTON_3    2 // GPIO34
#define BUTTON_2    0 // GPIO23
#define BUTTON_1    4 // GPIO22

Scheduler userScheduler;

void buttonPress_one(void);
void buttonPress_two(void);
void buttonPress_three(void);
void drawScreen(void);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

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
unsigned int numItems = 359;
unsigned int numItemsSet = 1;
unsigned int numItemsSetLength = 0;

bool button3Press = false;

Task drawUI(TASK_SECOND * 1, TASK_FOREVER, &drawScreen);

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  Serial.begin(9600);

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

void loop()
{
  userScheduler.execute();
}

char buffer[16];

void drawScreen(void){
  lcd.clear();
  switch(screenState){
    case(HOME):
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("        ITEMS   ");
      lcd.setCursor(3, 0);
      lcd.print(String(numItems));
      lcd.setCursor(0, 1);
      lcd.print("zero  menu  disp");
      break;
    case(MENU_EXIT):
      lcd.setCursor(0, 0);
      lcd.print("      EXIT      ");
      lcd.setCursor(0, 1);
      lcd.print("<   continue   >");
      break;
    case(MENU_SET_MIN):
      lcd.setCursor(0, 0);
      lcd.print("  SET MIN LEVL  ");
      lcd.setCursor(0, 1);
      lcd.print("<   continue   >");
      break;
    case(MENU_SET_WEIGHT):
      lcd.setCursor(0, 0);
      lcd.print("  SET  WEIGHT  ");
      lcd.setCursor(0, 1);
      lcd.print("<   continue   >");
      break;
    case(MENU_SET_NUM):
      lcd.setCursor(0, 0);
      lcd.print(" SET  NUM  PER ");
      lcd.setCursor(0, 1);
      lcd.print("<   continue   >");
      break;
    case(MENU_NUMBER_SCREEN):
      numItemsSetLength = (int)log10(numItemsSet) + 1;
      sprintf(buffer, "%*d", 8, numItemsSet);
      lcd.setCursor(0, 0);
      lcd.print(buffer);
      lcd.setCursor(0, 1);
      lcd.print("-1     ok     +1");
      break;
    default:
      // Sleep function
      lcd.noBacklight();
  }
}

void buttonPress_three(){
  if((millis() - button1LastPress) > debounceDelay){
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
