#include <Arduino.h>

char appData;  
String inData = "";
void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("Setup Default");
  Serial1.begin(9600);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("Setup One");
  pinMode(13, OUTPUT); // onboard LED
  digitalWrite(13, LOW); // switch OFF LED

  /**
 * Setup for HM-10
 * AT+IBEA1 // Set iBeacon on
 * AT+MARJ0xXXXX // Set Major value
 * AT+MINO0xXXXX // Set Minor value
 * // Set the iBeacon value
 * AT+IBE0 XXXXXXXX
 * AT+IBE1 XXXXXXXX
 * AT+IBE2 XXXXXXXX
 * AT+IBE3 XXXXXXXX
 */
}

bool hasRead = false;
bool hasReadHM = false;

void loop()
{
  while (Serial1.available() > 0) {   // if HM10 sends something then read
    Serial.write(Serial1.read());
    hasReadHM = true;
  }
 
  while (Serial.available() > 0) {           // Read user input if available.
    if(hasReadHM){
      Serial.println();
      hasReadHM = false;
    }
    appData = Serial.read();
    if(appData == 's'){
      Serial1.write("AT+IBEA1");
      delay(100);
 
      Serial1.write("AT+MARJ0x0001");
      delay(100);
      Serial1.write("AT+MINO0x0003");
      delay(100);

      Serial1.write("AT+IBE005DF65F2");
      delay(100);
      Serial1.write("AT+IBE1B5A111EC");
      delay(100);
      Serial1.write("AT+IBE2B9090242");
      delay(100);
      Serial1.write("AT+IBE3AC120002");
      delay(100);
    }else{
      Serial.write(appData);
      Serial1.write(appData);
    }
    hasRead=true;
  }
  if(hasRead){
    Serial.println();
    hasRead = false;
  }
}
