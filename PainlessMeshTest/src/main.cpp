#include "painlessMesh.h"

#define   MESH_PREFIX     "scale"
#define   MESH_PASSWORD   "scalepword"
#define   MESH_PORT       5555

#define PIN_BUTTON    34 // GPIO34
#define PIN_LED_RED   23 // GPIO23
#define PIN_LED_BLUE  22 // GPIO22

bool flashBlue = true;

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void flashNoConnections();
void flashRedSingleOn();
void flashRedSingleOff();

Task taskFlashBlue(TASK_SECOND * 1, TASK_FOREVER, &flashNoConnections);
Task flashRedOn(TASK_SECOND * 1, TASK_ONCE, &flashRedSingleOn);
Task flashRedOff(TASK_SECOND * 1, TASK_ONCE, &flashRedSingleOff);

#define MIN_BUTTON_PRESS 50

unsigned long lastPressed;

void sendMessage() {
  if(lastPressed == 0 || millis() - lastPressed >= MIN_BUTTON_PRESS){
    Serial.println("Interrupt Called");
    String msg = "Hi from node ";
    msg += mesh.getNodeId();
    mesh.sendBroadcast( msg );
  }

  lastPressed = millis();
}

void flashNoConnections(){
  digitalWrite(PIN_LED_BLUE, !digitalRead(PIN_LED_BLUE));
}

void flashRedSingleOn(){
  Serial.println("Flash On");
  digitalWrite(PIN_LED_RED, HIGH);
  if(flashRedOff.isEnabled()){
    flashRedOff.restartDelayed(TASK_SECOND);
  }else{
    flashRedOff.enableDelayed(TASK_SECOND);
  }
}

void flashRedSingleOff(){
  Serial.println("Flash Off");
  digitalWrite(PIN_LED_RED, LOW);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  if(flashRedOn.isEnabled()){
    flashRedOn.restart();
  }else{
    flashRedOn.enable();
  }
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  if(taskFlashBlue.isEnabled()){
    taskFlashBlue.disable();
    digitalWrite(PIN_LED_BLUE, HIGH);
  }
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  lastPressed = 0;

  // Set our Pin Modes
  pinMode(PIN_BUTTON, INPUT_PULLDOWN);     // Button Input
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), sendMessage, RISING);
  pinMode(PIN_LED_RED, OUTPUT);   // Red LED Output
  pinMode(PIN_LED_BLUE, OUTPUT);  // Blue LED Output

  digitalWrite(PIN_LED_BLUE, HIGH);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskFlashBlue);
  taskFlashBlue.enable();
  userScheduler.addTask(flashRedOn);
  userScheduler.addTask(flashRedOff);
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}