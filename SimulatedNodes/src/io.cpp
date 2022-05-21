#include "io.hpp"

Task ledFlashBlue(TASK_SECOND * 1, TASK_FOREVER, &flashBlueLed);
Task ledFlashRed(TASK_SECOND * 1, TASK_FOREVER, &flashRedLed);

void setupIO(Scheduler userScheduler){
    // Set our Pin Modes
    pinMode(PIN_BUTTON_1, INPUT_PULLDOWN);  // Button Input
    pinMode(PIN_BUTTON_2, INPUT_PULLDOWN);  // Button Input
    pinMode(PIN_LED_RED, OUTPUT);           // Red LED Output
    pinMode(PIN_LED_BLUE, OUTPUT);          // Blue LED Output

    attachInterrupt(PIN_BUTTON_1, button1Press, RISING);
    attachInterrupt(PIN_BUTTON_2, button2Press, RISING);

    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_BLUE, LOW);

    Serial.write("Setup IO");
    userScheduler.addTask(ledFlashBlue);
    ledFlashBlue.enable();
    userScheduler.addTask(ledFlashRed);
    ledFlashRed.enableDelayed(TASK_SECOND * 1);

    ledFlashBlue.isEnabled();
}

void setBlueLED(void){
    if(ledFlashBlue.isEnabled()){
        ledFlashBlue.disable();
    }

    digitalWrite(PIN_LED_BLUE, HIGH);
}

void setRedLED(void){
    if(ledFlashRed.isEnabled()){
        ledFlashRed.disable();
    }

    digitalWrite(PIN_LED_RED, HIGH);
}

long button1Last = 0;
long button2Last = 0;

void button1Press(void){
    if(millis() - button1Last > 500){
        button1Last = millis();
        updateRandomNumStored();
    }
}

void button2Press(void){
    if(millis() - button2Last > 500){
        button2Last = millis();
        updateRandomSettings();
    }
}

void startBlueFlash(void){
    if(!ledFlashBlue.isEnabled()){
        ledFlashBlue.enable();
    }
}

void stopBlueFlash(void){
    if(ledFlashBlue.isEnabled()){
        ledFlashBlue.disable();
    }
}

void startRedFlash(void){
    if(!ledFlashRed.isEnabled()){
        ledFlashRed.enable();
    }
}

void stopRedFlash(void){
    if(ledFlashRed.isEnabled()){
        ledFlashRed.disable();
    }
}

void flashBlueLed(){
    digitalWrite(PIN_LED_BLUE, !digitalRead(PIN_LED_BLUE));
}

void flashRedLed(){
    digitalWrite(PIN_LED_RED, !digitalRead(PIN_LED_RED));
}