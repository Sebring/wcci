
#define ESP_RESET 9
#define GREEN 12
#define RED 10
#define YELLOW 11
#define LED 13
#define SWITCH 4

#define BLINK_COUNT 1
#define BLINK_DURATION 500

String commandLock = "GET /log_visit_state/0 HTTP/1.0\r\n\r\n";
String commandUnlock = "GET /log_visit_state/1 HTTP/1.0\r\n\r\n";

int count;
int switchState;

void setup()
{
  pinMode(ESP_RESET, INPUT);
  //pinMode(ESP_RESET, OUTPUT);
  //digitalWrite(ESP_RESET, HIGH);
  
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);
  
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
  int state = digitalRead(SWITCH);
     
  if (switchState != state) {
    // send state to server
    if (state == LOW) {
      sendLock();
    } else {
      sendUnlock();
    }
  }

  switchState = state;

  if (switchState == LOW) {
    count++;
    digitalWrite(RED, HIGH);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);
  } else {
    if (count > 0) {
      count -= 6;
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, HIGH);
      digitalWrite(GREEN, LOW);
      
    } else {
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, LOW);
      digitalWrite(GREEN, HIGH);
    }
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


bool sendLock() {
  if (!sendATConnectServer()) return false;
  if (!sendATPrepareLockDoor()) return false;
  if (!sendATLockDoor()) return false;
  if (!sendATCipClose()) return false;
  return true;
}

bool sendUnlock() {
  if (!sendATConnectServer()) return false;
  if (!sendATPrepareUnlockDoor()) return false;
  if (!sendATUnlockDoor()) return false;
  if (!sendATCipClose()) return false;
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

