#include "Arduino.h"

#define ESP_RESET 9

#define RED 10
#define YELLOW 11
#define GREEN 12
#define LED 13
#define SWITCH 3

#define BLINK_COUNT 1
#define BLINK_DURATION 500

String commandLock = "GET /log_visit_state/0 HTTP/1.0\r\n\r\n";
String commandUnlock = "GET /log_visit_state/1 HTTP/1.0\r\n\r\n";

int count;
int lastState;
int courtesyDelay = 6;
long lastStateTimestamp;


// sometimes you want to run with leds only
boolean espActive = false;

void setup() {

  pinMode(ESP_RESET, INPUT);
  //pinMode(ESP_RESET, OUTPUT);
  //digitalWrite(ESP_RESET, HIGH);
  
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);

  // setup the switch
  pinMode(SWITCH, INPUT_PULLUP);
  lastState = digitalRead(SWITCH);
  lastStateTimestamp = millis();
  delay(100);
  attachInterrupt(digitalPinToInterrupt(SWITCH), onLockStateChange, CHANGE);

  digitalWrite(GREEN, HIGH);

  if (!espActive)
    return; // bail to loop
  
  // setup serial with esp-01
  digitalWrite(LED, HIGH);
  Serial.begin(115200);
  Serial.setTimeout(10000);

  blink(BLINK_COUNT, BLINK_DURATION, 1, 0, 0);

  if (!sendAT()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 0, 1, 0);

  if (!sendATReset()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 1, 1, 0);

  if (!sendATModeStation()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 0, 0, 1);

  if (!sendATReset()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 1, 0, 1);

  if (!sendATMultipleConnections()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 0, 1, 1);
  
  if (!sendATJoinNetwork()) blinkLEDForever();
  blink(BLINK_COUNT, BLINK_DURATION, 1, 1, 1);
}



void loop() {
  if (lastState == LOW) {
    count++;
  } else if (count > 0) { 
      count -= courtesyDelay;
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, HIGH);
      digitalWrite(GREEN, LOW);
  } else {
      count = 0;
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, LOW);
      digitalWrite(GREEN, HIGH);
  }
  
  delay(500);
}

void blink(int count, int duration, int r, int y, int g) {
  for(uint16_t i=0; i<count; i++) {
    digitalWrite(GREEN, g);
    digitalWrite(YELLOW, y);
    digitalWrite(RED, r);
    delay(duration);

    digitalWrite(GREEN, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);
    delay(duration);
  }
}

void onLockStateChange() {
  long now = millis();
  int state = digitalRead(SWITCH);
  if (state != lastState && now - lastStateTimestamp > 50) {
    lastStateTimestamp = millis();
    lastState = state;
    if (lastState == LOW) {
      sendLock();
    } else {
      sendUnlock();
    }
  }
}

bool sendLock() {
  
  digitalWrite(RED, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);
  
  if (espActive) {
    if (!sendATConnectServer()) return false;
    if (!sendATPrepareLockDoor()) return false;
    if (!sendATLockDoor()) return false;
    if (!sendATCipClose()) return false;
  }
  return true;
}

bool sendUnlock() {

  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(GREEN, LOW);
  
  if (espActive) {
    if (!sendATConnectServer()) return false;
    if (!sendATPrepareUnlockDoor()) return false;
    if (!sendATUnlockDoor()) return false;
    if (!sendATCipClose()) return false;
  }
  return true;
}

void blinkLEDForever() { 
  while(1) {
   digitalWrite(LED, HIGH);
   delay(100);
   digitalWrite(LED, LOW);
   delay(100);
  }
}

